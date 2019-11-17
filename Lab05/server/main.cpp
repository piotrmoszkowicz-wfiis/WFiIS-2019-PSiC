#include <iostream>
#include "server.hpp"

int main() {
    const char *HOST = "127.0.0.1";
    const uint16_t PORT = 65280;

    auto tcp_server = new Server(HOST, PORT);

    tcp_server->allocate_descriptor();

    in_addr address_struct = Server::convert_to_in_addr(HOST);
    sockaddr_in socket_address = Server::convert_to_sockeraddr_in(AF_INET, PORT, address_struct);

    tcp_server->bind_descriptor(socket_address);
    tcp_server->listen();

    sockaddr_in client_address = {};
    tcp_server->accept(client_address);

    tcp_server->get_key();
    tcp_server->send_key();

    /* auto message = tcp_server->receive_data();
    auto client_data = Server::get_client_data(client_address);

    std::cout << "address=" << client_data.addr << " port=" << client_data.port << " protocol=" << client_data.protocol
              << std::endl;
    std::cout << "Message: " << std::endl << message << std::endl;

    tcp_server->send_data(message); */
    tcp_server->close();

    delete tcp_server;

    return 0;
}