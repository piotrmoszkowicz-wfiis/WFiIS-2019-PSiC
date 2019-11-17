#include <iostream>
#include "server.hpp"

bool wannaClose = false;

void closeHandler(int a) {
    wannaClose = true;
}

int main() {
    signal(SIGTERM, closeHandler);

    const char *HOST = "127.0.0.1";
    const uint16_t PORT = 65280;

    auto tcp_server = new Server(HOST, PORT);

    tcp_server->allocate_descriptor();
    tcp_server->set_descriptor_flag(O_NONBLOCK);

    in_addr address_struct = Server::convert_to_in_addr(HOST);
    sockaddr_in socket_address = Server::convert_to_sockeraddr_in(AF_INET, PORT, address_struct);

    tcp_server->bind_descriptor(socket_address);
    tcp_server->listen();

    auto server_descriptor = tcp_server->get_server_descriptor();

    while (true) {
        if (wannaClose) {
            break;
        }
        int number_of_descriptors_with_event = tcp_server->poll(100);

        if (number_of_descriptors_with_event > 0) {
            auto descriptors = tcp_server->get_descriptors();
            int index = 0;
            for (auto descriptor : descriptors) {
                if (descriptor.revents & POLLIN) {
                    if (descriptor.fd == server_descriptor.fd) {
                        tcp_server->accept();
                    } else {
                        auto receiveData = Server::receive_data(descriptor.fd);
                        std::cout << "Message: " << std::endl << receiveData->m_buffer << std::endl << "Length:" << receiveData->m_length << std::endl;
                        if (receiveData->m_length == 0) {
                            Server::close(descriptor.fd);
                            descriptors.erase(descriptors.begin() + index); // TODO: Fix range loop
                        } else {
                            Server::send_data(descriptor.fd, receiveData->m_buffer);
                        }

                        delete receiveData;
                    }
                }
                index++;
            }
        }
    }

    delete tcp_server;

    return 0;
}