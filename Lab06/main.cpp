#include <iostream>

#include "TCP/server.hpp"

#include "HTTP/handler.hpp"

bool wannaClose = false;

void closeHandler(int a) {
    (void)a;
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
        int number_of_descriptors_with_event = tcp_server->poll(300);

        if (number_of_descriptors_with_event > 0) {
            auto descriptors = tcp_server->get_descriptors();
            int index = 0;
            for (auto descriptor : descriptors) {
                if (descriptor.m_descriptor.revents & POLLIN) { //NOLINT
                    if (descriptor.m_descriptor.fd == server_descriptor.fd) {
                        tcp_server->accept();
                    } else {
                        std::string request_buffer;

                        do {
                            std::cout << descriptor.m_ssl << std::endl;
                            auto receive_data = Server::receive_data(descriptor.m_ssl);

                            if (receive_data->m_length == 0) {
                                Server::close(descriptor.m_descriptor.fd);
                                descriptors.erase(descriptors.begin() + index);
                                break;
                            } else if (receive_data->m_length == -1) {
                                std::cout << "len = -1" << std::endl;
                                delete receive_data;
                                break;
                            } else {
                                request_buffer += receive_data->m_buffer;
                            }

                            delete receive_data;
                        } while(SSL_pending(descriptor.m_ssl));

                        if (!request_buffer.empty()) {
                            auto http_handler = new HTTPHandler(request_buffer);
                            auto response = http_handler->handle_request();
                            if (response.m_should_respond) {
                                // TODO: How to fix probably '\0' char in the images? When using .c_str() response gets bad :<
                                Server::send_data(descriptor.m_ssl, response.m_response.c_str());
                            }

                            delete http_handler;
                            SSL_free(descriptor.m_ssl);
                        }
                    }
                }
                index++;
            }
        }
    }

    delete tcp_server;

    return 0;
}