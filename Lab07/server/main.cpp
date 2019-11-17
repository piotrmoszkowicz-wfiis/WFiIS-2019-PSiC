//
// Created by Piotr Moszkowicz on 13/11/2019.
//

#include "zmq.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    void *context = zmq_ctx_new();
    void *receiver = zmq_socket(context, ZMQ_PULL);

    int rc = zmq_bind(receiver, "tcp://127.0.0.1:65280");

    while (true) {
        zmq_msg_t message;

        auto recv_res = zmq_msg_recv(&message, receiver, 0);
        if (recv_res == -1) {
            std::cout << "err recv_res" << std::endl << zmq_strerror(zmq_errno());
            exit(0);
        }

        auto data_res = zmq_msg_get(&message, ZMQ_SRCFD);
        auto msg_content = zmq_msg_data(&message);

        sockaddr_in client_addr;
        socklen_t size_client_addr = sizeof(client_addr);

        auto fd_data_res = getpeername(data_res, (sockaddr *) &client_addr, &size_client_addr);

        if (fd_data_res == 0) {
            std::cout << "err" << std::endl;
            exit(0);
        }

        if (msg_content == nullptr) {
            std::cout << "err" << std::endl;
            exit(0);
        }

        std::cout << "from " << client_addr.sin_addr.s_addr << ":" << client_addr.sin_port << "\t recived \t"
                  << msg_content << std::endl;

        zmq_msg_close(&message);
    }

    zmq_close(receiver);
    zmq_ctx_destroy(context);

    return 0;
}