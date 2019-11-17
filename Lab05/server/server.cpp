#include <iostream>

#include "server.hpp"

in_addr Server::convert_to_in_addr(const char *host) {
    in_addr address_struct = {};
    int address_success = inet_aton(host, &address_struct);
    error_handler("Server/convert_to_in_addr/inet_aton", address_success, false);
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
    error_handler("Server/get_client_data/getaddrinfo", addr_info_success, false, false);

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
    m_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    error_handler("Server/allocate_descriptor", m_descriptor);
    std::cout << "Descriptor has been allocated" << std::endl;
}

void Server::bind_descriptor(sockaddr_in &addr) {
    int bind_result = bind(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler("Server/BindDescriptor", bind_result);
    std::cout << "Description has been bound" << std::endl;
}

void Server::listen() {
    int listen_result = ::listen(m_descriptor, SOMAXCONN);
    error_handler("Server/listen", listen_result);
    std::cout << "Description is listening" << std::endl;
}

void Server::accept(sockaddr_in &client_addr) {
    socklen_t size_of_addr = sizeof(client_addr);
    m_current_connection_descriptor = ::accept(m_descriptor, (sockaddr *) &client_addr, &size_of_addr);
    error_handler("Server/accept", m_current_connection_descriptor);
    std::cout << "We have new descriptor to accept connections on. Let's start generating the key" << std::endl;
}

void Server::send_data(const char *message) {
    std::string msg_str = message;
    msg_str += "\0";
    int full_send_length = 0;
    int length_to_send = msg_str.size();
    const char* msg_char = msg_str.c_str();

    while (length_to_send != full_send_length) {
        ssize_t send_length = send(m_current_connection_descriptor, msg_char + full_send_length, length_to_send - full_send_length, 0);
        error_handler("Server/send_data/send", send_length, true);
        full_send_length += send_length;
    }
    ssize_t send_length = send(m_current_connection_descriptor, "\0", 1, 0);
    error_handler("Server/send_data/send", send_length, true);
    std::cout << "Sending out data..." << std::endl;
}

std::string& Server::receive_data() {
    auto buff = new char[128];
    m_receive_buffer.clear();

    while (true) {
        ssize_t receive_length = recv(m_current_connection_descriptor, (void *) buff, 128,
                                     0);

        m_receive_buffer += buff;

        if (receive_length == 0 || (receive_length == 1 && buff[0] == '\0')) {
            break;
        }
        error_handler("Server/receive_data", receive_length);
    }

    delete [] buff;

    return m_receive_buffer;
}

void Server::close() {
    int close_result = ::close(m_descriptor);
    error_handler("Server/close", close_result);
    std::cout << "Closing myself... :)" << std::endl;
}

void Server::error_handler(const char *invoker, int result, bool onMinusOne, bool eq) {
    if ((eq && onMinusOne && result == -1) || (eq && !onMinusOne && result == 0) ||
        (!eq && !onMinusOne && result != 0)) {
        std::cout << "Got error during running the program from function [" << invoker << "]: ";
        if (errno != 0) {
            std::cout << std::strerror(errno);
        } else {
            auto err = ERR_get_error();
            std::cout << ERR_error_string(err, nullptr) << std::endl;
        }
        std::cout << std::endl;
        if (std::strcmp(invoker, "Server/BindDescriptor") == 0) {
            exit(0);
        }
    }
}

void Server::send_key() {
    int generate_key_result = DH_generate_key(m_key);
    error_handler("Server/send_key/generate_key_result", generate_key_result);

    const char *key_char = BN_bn2hex(m_key->pub_key);

    send_data(key_char);
}

void Server::get_key() {
    std::string message = receive_data();

    auto public_key = new BIGNUM();

    std::cout << "Got key: " << message << std::endl;

    int conversion_result = BN_hex2bn(&public_key, message.c_str());
    error_handler("Server/get_key/conversion_result", conversion_result);

    std::cout << "PK" << public_key << std::endl;

    int compute_key_result = DH_compute_key(m_secret, public_key, m_key);
    error_handler("Server/get_key/compute_key_result", compute_key_result);

    std::cout << "Secret: " << std::endl << m_secret << std::endl;

    delete public_key;
}