#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cerrno>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

/**
 * @struct ClientData
 * @brief Structure which includes needed TCP ClientData
 */
struct ClientData {
    const char *addr; /** IP Address **/
    const uint16_t port; /** Port **/
    const int protocol; /** Protocol ID **/
};

struct ReceiveData {
    ReceiveData(int length, const char *buffer) : m_length(length), m_buffer(buffer) {}

    ~ReceiveData() {
        delete[] m_buffer;
    }

    int m_length;
    const char *m_buffer;
};

/**
 * @class Server
 * @brief Class which implements TCP server functionality
 */
class Server {
public:
    /**
     * @fn Server
     * @brief TCP Server constructor
     * @param [const char *]    server_host - IP Address of TCP Server
     * @param [cinst uint16_t]  server_port - Port of TCP Server
     */
    Server(const char *server_host, const uint16_t server_port) : m_server_host(server_host),
                                                                  m_server_port(server_port),
                                                                  m_descriptor(pollfd{}) {
        m_descriptors = std::vector<pollfd>{};
    }

    /**
     * @fn ~Server
     * @brief Destructor of Server class
     */
    ~Server() {
        Server::close(m_descriptor.fd);
        std::cout << "Main server is closed..." << std::endl;
    }

    /**
     * @fn convert_to_in_addr
     * @brief Converts IP Address to correct network notation
     * @param [const char*] host          - IP address to convert
     * @return [in_addr]                  - Input address structure
     */
    in_addr static convert_to_in_addr(const char *host);

    /**
     * @fn convert_to_sockeraddr_in
     * @brief Converts IP Address and port to correct socket notation
     * @param [sa_family_t] family        - Family of protocol
     * @param [in_port_t]   port          - Port number
     * @param [in_addr]     addr          - IP Address struct
     * @return [sockaddr_in]              - Socket address structure
     */
    sockaddr_in static convert_to_sockeraddr_in(sa_family_t family, in_port_t port, in_addr &addr);

    /**
     * @fn get_client_data
     * @brief Returns data about client's connection in our structured way
     * @param [sockaddr_in] client_addr   - Client's address socket structure
     * @return [ClientData]               - Structured client data
     */
    ClientData static get_client_data(sockaddr_in &client_addr);

    /**
     * @fn error_handler
     * @brief Handles errno network errors
     * @param [const char*] invoker     - Invoker of error handler
     * @param [int] result              - Result of operation
     * @param [bool] onMinusOne         - Whether handle if result is -1 or other
     * @param [bool] eq                 - Whether handle if result is equal to -1/other or not equal
     */
    void static error_handler(const char *invoker, int result, bool onMinusOne = true, bool eq = true);

    /**
     * @fn send_data
     * @brief Sends data to the socket
     * @param [const char*] message       - Data to send
     */
    void static send_data(int descriptor, const char *message);

    /**
     * @fn receive_data
     * @brief Receives data from the socket
     * @return [char*]                    - Data sent to socket by client
     */
    static ReceiveData *receive_data(int descriptor);

    /**
     * @fn close
     * @brief Closes TCP Socket
     */
    void static close(int descriptor);

    /**
     * @fn allocate_descriptor
     * @brief Allocates socket descriptor
     */
    void allocate_descriptor();

    /**
     * @fn bind_descriptor
     * @brief Binds socket descriptor
     * @param [sockaddr_in&] addr         - Address to bind to
     */
    void bind_descriptor(sockaddr_in &addr);

    /**
     * @fn listen
     * @brief Listen to incoming connections
     */
    void listen();

    /**
     * @fn accept
     * @brief Accept certain connection and create new descriptor for it
     * @param [sockaddr_in&] client_addr  - Client's address
     */
    int accept();

    /**
     * @fn set_descriptor_flag
     * @brief Sets certain descriptor flag
     * @param flag                        - Flag to set
     */
    void set_descriptor_flag(int flag);

    /**
     * @fn poll
     * @brief Checks what descriptors have changed
     * @param timeout                     - Timeout in miliseconds
     * @return [int]                      - Number of descriptors changed
     */
    int poll(int timeout);


    /**
     * @fn get_server_descriptor
     * @return
     */
    pollfd &get_server_descriptor() {
        return m_descriptor;
    }

    /**
     * @fn get_descriptors
     * @brief Gets vector of descriptors
     * @return [std::vector<pollfd>]    - Vector of descriptors
     */
    std::vector<pollfd> &get_descriptors() {
        return m_descriptors;
    }

private:
    const char *m_server_host; /** Server HOST IP **/
    const uint16_t m_server_port; /** Server PORT **/
    pollfd m_descriptor; /** Server descriptor **/
    std::vector<pollfd> m_descriptors; /** Pool of descriptors to check on **/
};