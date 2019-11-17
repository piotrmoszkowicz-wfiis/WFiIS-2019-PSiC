#include <iostream>

#include "client.hpp"

in_addr Client::convert_to_in_addr(const char *host) {
    in_addr address_struct = {};
    int address_success = inet_aton(host, &address_struct);
    error_handler("Client/convert_to_in_addr/inet_aton", address_success, false);
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
    error_handler("Client/allocate_descriptor", m_descriptor);
    std::cout << "Descriptor has been allocated" << std::endl;
}

void Client::bind_descriptor(sockaddr_in &addr) {
    int bind_result = bind(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler("Client/BindDescriptor", bind_result);
    std::cout << "Description has been bound" << std::endl;
}

void Client::connect_to_server(sockaddr_in &addr) {
    int connect_result = connect(m_descriptor, (sockaddr *) &addr, sizeof(addr));
    error_handler("Client/connect_to_server/connect", connect_result, true);
}

void Client::send_data(const char *message) {
    std::string msg_str = message;
    msg_str += "\0";
    int full_send_length = 0;
    int length_to_send = msg_str.size();
    const char* msg_char = msg_str.c_str();

    while (length_to_send != full_send_length) {
        ssize_t send_length = send(m_descriptor, msg_char + full_send_length, length_to_send - full_send_length, 0);
        error_handler("Client/send_data/send", send_length, true);
        full_send_length += send_length;
    }
    ssize_t send_length = send(m_descriptor, "\0", 1, 0);
    error_handler("Client/send_data/send", send_length, true);
    std::cout << "Sending out data..." << std::endl;
}

std::string& Client::receive_data() {
    auto buff = new char[1024];
    m_receive_buffer.clear();

    while (true) {
        ssize_t recive_length = recv(m_descriptor, (void *) buff, 1024,
                                     0);
        if (recive_length == 0 || (recive_length == 1 && buff[0] == '\0')) {
            break;
        }
        error_handler("Client/receive_data", recive_length);

        m_receive_buffer += buff;
    }

    delete [] buff;

    return m_receive_buffer;
}

void Client::close() {
    int close_result = ::close(m_descriptor);
    error_handler("Client/close/close", close_result, true);
    std::cout << "Closing myself... :)" << std::endl;
}

void Client::error_handler(const char *invoker, int result, bool onMinusOne, bool eq) {
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
        if (std::strcmp(invoker, "Client/BindDescriptor") == 0) {
            exit(0);
        }
    }
}

void Client::send_key() {
    int generate_key_result = DH_generate_key(m_key);
    error_handler("Client/send_key/generate_key_result", generate_key_result);

    const char *key_char = BN_bn2hex(m_key->pub_key);

    send_data(key_char);
}

void Client::get_key() {
    std::string message = receive_data();

    auto public_key = new BIGNUM();

    std::cout << "Got key: " << message << std::endl;

    int conversion_result = BN_hex2bn(&public_key, message.c_str());
    error_handler("Client/get_key/conversion_result", conversion_result);

    std::cout << "PK" << public_key << std::endl;

    int compute_key_result = DH_compute_key(m_secret, public_key, m_key);
    error_handler("Client/get_key/compute_key_result", compute_key_result);

    std::cout << "Secret: " << std::endl << m_secret << std::endl;

    delete public_key;
}