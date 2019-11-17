//
// Created by Piotr Moszkowicz on 13/11/2019.
//

#include "zmq.hpp"

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

std::string getTime (const char* format)
{
    time_t rawtime;
    struct tm* timeinfo;
    char output[40];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    std::stringstream formatString;
    formatString<<"server time: "<<format;
    strftime (output, sizeof(output), formatString.str().data(), timeinfo);

    return output;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Wrong args!" << std::endl;
        exit(0);
    }

    const std::string PORT = argv[0];
    const std::string REGEX = argv[1];
    const std::string SLEEP_TIME_SEC = argv[2];
    const std::string NUM = argv[3];

    const int SLEEP_TIME_MILISEC = std::stoi(SLEEP_TIME_SEC) * 1000;
    const int NUM_INT = std::stoi(NUM);

    const std::string addr = "tcp://localhost:" + PORT;

    void* context = zmq_ctx_new();
    void* requester = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(requester, addr.c_str());

    for (int i = 0; i < NUM_INT; i++) {
        std::string time = getTime(REGEX.c_str());

        std::cout << "Sending ..." << time << std::endl;
        zmq_send(requester, time.c_str(), time.length() + 1, 0);

        sleep(SLEEP_TIME_MILISEC);
    }

    zmq_close(requester);
    zmq_ctx_destroy(context);

    return 0;
}