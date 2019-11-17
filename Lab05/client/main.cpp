//
// Created by Piotr Moszkowicz on 2019-10-06.
//

#include "client.hpp"

int main() {
    const char *HOST = "127.0.0.1";
    const uint16_t SERVER_PORT = 65280;
    const uint16_t CLIENT_PORT = 65285;

    auto tcp_client = new Client(HOST, CLIENT_PORT, HOST, SERVER_PORT);

    tcp_client->allocate_descriptor();

    in_addr address_struct = Client::convert_to_in_addr(HOST);
    sockaddr_in socket_address = Client::convert_to_sockeraddr_in(AF_INET, CLIENT_PORT, address_struct);

    tcp_client->bind_descriptor(socket_address);

    sockaddr_in host_socket_address = Client::convert_to_sockeraddr_in(AF_INET, SERVER_PORT, address_struct);

    tcp_client->connect_to_server(host_socket_address);
    tcp_client->send_key();
    tcp_client->get_key();
    /* tcp_client->send_data("test");
    auto message = tcp_client->receive_data();

    std::cout << "Message from server: " << std::endl;
    std::cout << message << std::endl; */

    tcp_client->close();

    delete tcp_client;

    return 0;
}