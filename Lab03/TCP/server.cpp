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
    m_descriptors.push_back(new_descriptor_pollfd);

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
    m_descriptors.push_back(new_descriptor_pollfd);
    return connection_descriptor;
}

void Server::send_data(int descriptor, const char *message) {
    std::cout << "Trying to send out data" << std::endl;
    ssize_t send_length = send(descriptor, message, (strlen(message) + 1) * sizeof(char), 0);
    error_handler("SendData", send_length);
    std::cout << "Data has been sent" << std::endl;
}

void Server::set_descriptor_flag(int flag) {
    int flag_set_result = fcntl(m_descriptor.fd, F_SETFL, flag);
    error_handler("SetDescriptorFlag", flag_set_result);
}

int Server::poll(int timeout) {
    int poll_result = ::poll(m_descriptors.data(), m_descriptors.size(), timeout);
    error_handler("Poll", poll_result);
    return poll_result;
}

ReceiveData *Server::receive_data(int descriptor) {
    auto buffer = new char[1024];
    ssize_t recive_length = recv(descriptor, (void *) buffer, sizeof(buffer),
                                 0);
    error_handler("ReceiveData", recive_length);
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
