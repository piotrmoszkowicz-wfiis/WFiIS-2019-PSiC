//
// Created by Piotr Moszkowicz on 20/10/2019.
//

#include "handler.hpp"

HandlerResponse HTTPHandler::handle_request() {
    std::cout << "Headers:" << std::endl;
    for (auto& header : m_request_headers) {
        std::cout << header.first << ":" << header.second << std::endl;
    }

    int response_code = 200;
    int content_length = 0;
    bool isImg = false;
    std::string extension = Utils::split_string(m_path, ".")[1];
    std::string response_data{};

    if (m_request_type == REQUEST_TYPE::GET) {
        if (m_path[m_path.size() - 1] == '/') {
            m_path += "index.html";
            extension = "html";
        }
        try {
            // TODO: Investigate why images are being properly sent
            auto file = Utils::read_file(("./static" + m_path).c_str());
            response_data = std::string(file.begin(), file.end());
        } catch (std::ios_base::failure& e) {
            response_code = 404;
            response_data = Utils::not_found_response();
        } catch (std::exception& e) {
            std::cout << "UNKNWN EXCEPT " << e.what() << std::endl;
        }
    } else if (m_request_type == REQUEST_TYPE::POST) {
        std::cout << "POST CONTENT: " << std::endl;
        std::cout << m_content << std::endl;
    }



    if (extension == "jpg") {
        isImg = true;
    }

    content_length = response_data.size();

    auto response = std::string{"HTTP/1.1 "};

    if (response_code == 200) {
        response += "200 OK\n";
    } else {
        response += "404 NOT FOUND\n";
    }

    response += Utils::get_http_date();
    response += "Server: 1337-server\n";
    response += "Content-Type: ";
    response += Utils::get_content_type(extension.c_str());
    response += ";charset=utf8\n";
    response += "Content-Length: " + std::to_string(content_length) + "\n";

    if (isImg) {
        response += "Connection: Keep-Alive\n\r\n";
    } else {
        response += "Connection: close\n\r\n";
    }

    response += response_data;

    return HandlerResponse(true, response);
}

void HTTPHandler::parse_request_details() {
    auto delimited_request = Utils::split_string(m_buffer, std::string("\n\r"));
    m_content = delimited_request[1];

    auto headers = Utils::split_string(delimited_request[0], std::string("\n"));
    m_request_headers = Utils::parse_request_details_to_headers_vector(headers);

    auto request_details = Utils::split_string(headers[0], " ");

    if (request_details[0] == "GET") {
        m_request_type = REQUEST_TYPE::GET;
    } else if (request_details[0] == "POST") {
        m_request_type = REQUEST_TYPE::POST;
    }

    m_path = Utils::split_string(request_details[1], std::string("?"))[0];
}
