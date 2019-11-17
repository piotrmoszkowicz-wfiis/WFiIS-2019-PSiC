#include <iostream>
#include <cstdlib>
#include <string>
#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/dh.h>
#include <openssl/err.h>

/**
 * @class Client
 * @brief Class which implements TCP client functionality
 */
class Client {
public:
    /**
     * @fn Server
     * @brief TCP Server constructor
     * @param [const char *]    client_host - IP Address of TCP Client
     * @param [cinst uint16_t]  client_port - Port of TCP Client
     * @param [const char *]    server_host - IP Address of TCP Server
     * @param [cinst uint16_t]  server_port - Port of TCP Server
     */
    Client(const char *client_host, const uint16_t client_port, const char *server_host, const uint16_t server_port)
            : m_client_host(client_host), m_client_port(client_port), m_server_host(server_host),
              m_server_port(server_port), m_descriptor(0) {
        m_receive_buffer = std::string{};
        m_key = DH_get_2048_256();
        m_secret = new unsigned char[2048];
    }

    ~Client() {
        delete [] m_secret;
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

    void connect_to_server(sockaddr_in &addr);

    /**
     * @fn send_data
     * @brief Sends data to the socket
     * @param [const char*] message       - Data to send
     */
    void send_data(const char *message);

    /**
     * @fn receive_data
     * @brief Receives data from the socket
     * @return [std::string&]                    - Data sent to socket by client
     */
    std::string& receive_data();

    /**
     * @fn close
     * @brief Closes TCP Socket
     */
    void close();

    void send_key();

    void get_key();

private:
    /**
     * @fn error_handler
     * @brief Handles errno network errors
     * @param [const char*] invoker     - Invoker of error handler
     * @param [int] result              - Result of operation
     * @param [bool] onMinusOne         - Whether handle if result is -1 or other
     * @param [bool] eq                 - Whether handle if result is equal to -1/other or not equal
     */
    void static error_handler(const char* invoker, int result, bool onMinusOne = true, bool eq = true);

    const char *m_client_host; /** Client HOST IP **/
    const uint16_t m_client_port; /** Client PORT **/
    const char *m_server_host; /** Server HOST IP **/
    const uint16_t m_server_port; /** Server PORT **/
    int m_descriptor; /** Client descriptor **/
    std::string m_receive_buffer; /** Message receive buffer **/
    DH *m_key;
    unsigned char* m_secret;
};