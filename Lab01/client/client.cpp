#include <iostream>

#include "client.hpp"

in_addr Client::convert_to_in_addr(const char *host) {
    in_addr address_struct = {};
    int address_success = inet_aton(host, &address_struct);
    error_handler(address_success, false);
    return address_struct;
}

sockaddr_in Client::convert_to_sockeraddr_in(sa_family_t family, in_port_t port, in_addr &addr) {
    sockaddr_in socket_address = {};
    socket_address.sin_family = family;
    // NOLINTNEXTLINE
    socket_address.sin_port = htons(port);
    socket_address.sin_addr = addr;
    return socket_address;
}

void Client::allocate_descriptor() {
    m_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    error_handler(m_descriptor, true);
    std::cout << "Descriptor has been allocated" << std::endl;
}

void Client::bind_descriptor(sockaddr_in &addr) {
    int bind_result = bind(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler(bind_result, true);
    std::cout << "Description has been bound" << std::endl;
}

void Client::connect_to_server(sockaddr_in &addr) {
    int connect_result = connect(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler(connect_result, true);
}

void Client::send_data(const char *message) {
    std::cout << "Trying to send out data " << std::endl;
    ssize_t send_length = send(m_descriptor, message, (strlen(message) + 1) * sizeof(char), 0);
    error_handler(send_length, true);
}

char *Client::receive_data() {
    ssize_t recive_length = recv(m_descriptor, (void *) m_receive_buffer, sizeof(m_receive_buffer), 0);
    error_handler(recive_length, true);
    return m_receive_buffer;
}

void Client::close() {
    int close_result = ::close(m_descriptor);
    error_handler(close_result, true);
    std::cout << "Closing myself... :)" << std::endl;
}

void Client::error_handler(int result, bool onMinusOne) {
    if ((onMinusOne && result == -1) || (!onMinusOne && result == 0)) {
        std::cout << "Got error during running the program" << std::endl;
        std::cout << errno << std::endl;
        exit(0);
    }
}
