//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../Utils/get_content_type.hpp"
#include "../Utils/get_date.hpp"
#include "../Utils/http_error_responses.hpp"
#include "../Utils/parse_headers.hpp"
#include "../Utils/read_file.hpp"
#include "../Utils/split_string.hpp"

/**
 * HandlerResponse structure, which defines all needed params of HTTP response
 */
struct HandlerResponse {
    /**
     * @fn HandlerResponse
     * @brief Creates new HandlerResponse structure
     * @param [bool]            should_response         - Whether should we respond or not
     * @param [std::string]     response                - Actual string response
     */
    HandlerResponse(bool should_response, std::string response) : m_should_respond(should_response),
                                                                  m_response(std::move(response)) {}

    bool m_should_respond;
    std::string m_response;
};

/**
 * Request Type Enum represents HTTP Methods
 */
// TODO: Add more methods
enum class REQUEST_TYPE {
    GET = 0,
    POST = 1
};

class HTTPHandler {
public:
    /**
     * @fn HTTPHandler
     * @brief Constructor of HTTPHandler class
     * @param [std::string&]    buffer                  - Buffer of data from client's request
     */
    explicit HTTPHandler(std::string &buffer) : m_buffer(buffer), m_request_type(REQUEST_TYPE::GET) {
        parse_request_details();
    }

    /**
     * @fn handle_request
     * @brief Handles single request
     * @return [HandlerResponse]                        - Response object details, that will be used by TCP server
     */
    HandlerResponse handle_request();

private:
    /**
     * @fn parse_request_details
     * @brief Assigns request details variables of class from buffer
     */
    void parse_request_details();

    std::string m_buffer;
    std::string m_path;
    std::string m_content;
    std::vector<std::pair<std::string, std::string>> m_request_headers;
    REQUEST_TYPE m_request_type;
};
