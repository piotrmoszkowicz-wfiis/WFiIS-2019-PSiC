#include <iostream>

#include "server.hpp"

in_addr Server::convert_to_in_addr(const char *host) {
    in_addr address_struct = {};
    int address_success = inet_aton(host, &address_struct);
    error_handler(address_success, false);
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
    error_handler(addr_info_success, false, false);

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
    error_handler(m_descriptor);
    std::cout << "Descriptor has been allocated" << std::endl;
}

void Server::bind_descriptor(sockaddr_in &addr) {
    int bind_result = bind(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler(bind_result);
    std::cout << "Description has been bound" << std::endl;
}

void Server::listen() {
    int listen_result = ::listen(m_descriptor, SOMAXCONN);
    error_handler(listen_result);
    std::cout << "Description is listening" << std::endl;
}

void Server::accept(sockaddr_in &client_addr) {
    socklen_t size_of_addr = sizeof(client_addr);
    m_current_connection_descriptor = ::accept(m_descriptor, (sockaddr *) &client_addr, &size_of_addr);
    error_handler(m_current_connection_descriptor);
    std::cout << "We have new descriptor to accept connections on" << std::endl;
}

void Server::send_data(const char *message) {
    std::cout << "Trying to send out data " << std::endl;
    ssize_t send_length = send(m_current_connection_descriptor, message, (strlen(message) + 1) * sizeof(char), 0);
    error_handler(send_length);
}

char *Server::receive_data() {
    ssize_t recive_length = recv(m_current_connection_descriptor, (void *) m_receive_buffer, sizeof(m_receive_buffer),
                                 0);
    error_handler(recive_length);
    return m_receive_buffer;
}

void Server::close() {
    int close_result = ::close(m_descriptor);
    error_handler(close_result);
    std::cout << "Closing myself... :)" << std::endl;
}

void Server::error_handler(int result, bool onMinusOne, bool eq) {
    if ((eq && onMinusOne && result == -1) || (eq && !onMinusOne && result == 0) ||
        (!eq && !onMinusOne && result != 0)) {
        std::cout << "Got error during running the program" << std::endl;
        std::cout << errno << std::endl;
        exit(0);
    }
}
