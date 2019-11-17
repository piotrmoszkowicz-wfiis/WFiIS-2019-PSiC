#include <iostream>

#include "server.hpp"

in_addr Server::convert_to_in_addr(const char *host) {
    in_addr address_struct = {};
    int address_success = inet_aton(host, &address_struct);
    error_handler("ConvertToInAddr", address_success, false);
    return address_struct;
}

sockaddr_in Server::convert_to_sockeraddr_in(sa_family_t family, in_port_t port, in_addr &addr) {
    sockaddr_in socket_address = {};
    socket_address.sin_family = family;
    // NOLINTNEXTLINE
    socket_address.sin_port = htons(port);
    socket_address.sin_addr = addr;
    return socket_address;
}

ClientData Server::get_client_data(sockaddr_in &client_addr) {
    // NOLINTNEXTLINE
    auto client_port_network = ntohs(client_addr.sin_port);
    std::string client_port_str = std::to_string(client_port_network);
    char *client_port_char = new char[client_port_str.length() + 1];
    std::strcpy(client_port_char, client_port_str.c_str());

    auto addr_info = new addrinfo;
    auto addr_info_tab = new addrinfo[5];

    int addr_info_success = getaddrinfo(inet_ntoa(client_addr.sin_addr), client_port_char, addr_info, &addr_info_tab);
    error_handler("GetClientData-GetAddrInfo", addr_info_success, false, false);

    ClientData client_data = {
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port), //NOLINT
            addr_info_tab[0].ai_protocol
    };

    delete[] client_port_char;
    delete addr_info;
    delete[] addr_info_tab;

    return client_data;
}

void Server::allocate_descriptor() {
    int server_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    error_handler("AllocateDescriptor-Socket", server_descriptor);
    int value = 1;
    int set_sock_opt_result = setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
    error_handler("AllocateDescriptor-SetSockOpt", set_sock_opt_result);
    pollfd new_descriptor_pollfd = {
            server_descriptor,
            POLLIN
    };
    m_descriptor.fd = server_descriptor;

    auto descriptor_with_ssl = new DescriptorWithSSL{
            new_descriptor_pollfd,
            nullptr
    };

    m_descriptors.push_back(*descriptor_with_ssl);

    std::cout << "Descriptor has been allocated" << std::endl;
}

void Server::bind_descriptor(sockaddr_in &addr) {
    int bind_result = bind(m_descriptor.fd, (sockaddr *) &addr, sizeof(addr));
    error_handler("BindDescriptor", bind_result);
    std::cout << "Description has been bound" << std::endl;
}

void Server::listen() {
    int listen_result = ::listen(m_descriptor.fd, SOMAXCONN);
    error_handler("Listen", listen_result);
    std::cout << "Description is listening" << std::endl;
}

int Server::accept() {
    // socklen_t size_of_addr = sizeof(client_addr);
    //int connection_descriptor = ::accept(m_descriptor.fd, (sockaddr *) &client_addr, &size_of_addr);
    int connection_descriptor = ::accept(m_descriptor.fd, nullptr, nullptr); // TODO: Save all address info too in array of descriptors

    error_handler("Accept", connection_descriptor);
    std::cout << "We have new descriptor to accept connections on" << std::endl;
    pollfd new_descriptor_pollfd = {
            connection_descriptor,
            POLLIN
    };

    SSL* ssl;
    ssl = SSL_new(this->get_ssl_ctx());
    SSL_set_accept_state(ssl);
    SSL_set_fd(ssl, new_descriptor_pollfd.fd);

    /* auto ssl_loc = new SSL();
    *ssl_loc = *ssl; */

    auto descriptor_with_ssl = new DescriptorWithSSL{
        new_descriptor_pollfd,
        ssl
    };

    m_descriptors.push_back(*descriptor_with_ssl);
    return connection_descriptor;
}

void Server::send_data(SSL* ssl, const char *message) {
    std::cout << "Trying to send out data" << std::endl;
    SSL_write(ssl, message, strlen(message) + 1);
    std::cout << "Data has been sent" << std::endl;
}

void Server::set_descriptor_flag(int flag) {
    int flag_set_result = fcntl(m_descriptor.fd, F_SETFL, flag);
    error_handler("SetDescriptorFlag", flag_set_result);
}

int Server::poll(int timeout) {
    int i = 0;
    auto descriptors_array = new pollfd[m_descriptors.size()];

    for (auto descriptor : m_descriptors) {
        descriptors_array[i] = descriptor.m_descriptor;
        i++;
    }

    int poll_result = ::poll(descriptors_array, m_descriptors.size(), timeout);
    error_handler("Poll", poll_result);

    for (i = 0; i < m_descriptors.size(); i++) {
        m_descriptors[i].m_descriptor = descriptors_array[i];
    }

    delete [] descriptors_array;

    return poll_result;
}

ReceiveData *Server::receive_data(SSL* ssl) {
    auto buffer = new char[1024];
    int recive_length = SSL_read(ssl, buffer, sizeof(buffer));

    if (recive_length == -1) {
        ERR_print_errors_fp(stderr);
    }

    auto receive_data = new ReceiveData(
            recive_length,
            buffer
            );
    return receive_data;
}

void Server::close(int descriptor) {
    int close_result = ::close(descriptor);
    error_handler("Close", close_result);
    std::cout << "Closing socket..." << std::endl;
}

void Server::error_handler(const char* invoker, int result, bool onMinusOne, bool eq) {
    if ((eq && onMinusOne && result == -1) || (eq && !onMinusOne && result == 0) ||
        (!eq && !onMinusOne && result != 0)) {
        std::cout << "Got error during running the program from function [" << invoker << "]: " << std::strerror(errno)  << std::endl;
        if (std::strcmp(invoker, "BindDescriptor") == 0) {
            exit(0);
        }
    }
}

void Server::init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void Server::create_context() {
    const SSL_METHOD *method;

    method = SSLv23_server_method();

    m_ssl_ctx = SSL_CTX_new(method);
    if (!m_ssl_ctx) {
        perror("Unable to create SSL context");
        auto err = ERR_get_error();
        std::cout << ERR_error_string(err, nullptr) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Server::configure_context() {
    SSL_CTX_set_ecdh_auto(m_ssl_ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(m_ssl_ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        auto err = ERR_get_error();
        std::cout << "ServerCrtErr" << ERR_error_string(err, nullptr) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(m_ssl_ctx, "server.key", SSL_FILETYPE_PEM) <= 0 ) {
        auto err = ERR_get_error();
        std::cout << "ServerKeyErr" << ERR_error_string(err, nullptr) << std::endl;
        exit(EXIT_FAILURE);
    }
}