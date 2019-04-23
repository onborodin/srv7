//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <utility>
#include <vector>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <utility>
#include <iomanip>

#include <chrono>
#include <thread>


#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace server {

connection::connection(asio::ip::tcp::socket socket, class connection_manager& manager, class request_handler& handler)
    : socket(std::move(socket)),
      connection_manager(manager),
      request_handler(handler) {
}

void connection::start() {
    do_read();
}

void connection::stop() {
    socket.close();
}

void connection::do_read() {
    auto self(shared_from_this());

    auto func = [this, self](std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            request_parser::result_type result;
            std::tie(result, std::ignore) = request_parser.parse(
                    request, buffer.data(), buffer.data() + bytes_transferred);

            if (result == request_parser::good) {
                request_handler.handle_request(request, reply);
                do_write();
            } else if (result == request_parser::bad) {
                reply = reply::stock_reply(reply::bad_request);
                do_write();
            } else {
                do_read();
            }
        } else if (ec != asio::error::operation_aborted) {
            connection_manager.stop(shared_from_this());
        }
    };

    socket.async_read_some(asio::buffer(buffer), func);
}


std::string timestamp() {
    std::stringstream ss;
    std::time_t now = std::time(0);
    ss << std::put_time(std::gmtime(&now), "%a, %d %b %Y %T %Z");
    return ss.str();
}

int readfile(std::string& name, std::string& content) {
    std::ifstream is(name, std::ifstream::binary);
    if (is) {
        std::filesystem::path p(name);
        int size = std::filesystem::file_size(p);

        char* buffer = new char[size];
        is.read(buffer, size);
        is.close();
        content = buffer;

        delete[] buffer;
        return size;
      }
    return -1;
}

void connection::do_write() {
    auto self(shared_from_this());
    auto func = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            // Initiate graceful connection closure.
            asio::error_code ignored_ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                ignored_ec);
        }
        if (ec != asio::error::operation_aborted) {
            connection_manager.stop(shared_from_this());
        }
    };

            std::string path = "./configure";

            std::string response;
            std::string body;

            std::stringstream header;
            size_t size = 0;
            if ((size = readfile(path, body)) > 0) {
                header << "HTTP/1.1 200 OK\r\n";
                header << "Content-Type: text/plain\r\n";
            } else {
                header << "HTTP/1.1 404 Not Found\r\n";
                header << "Content-Type: text/plain\r\n";
            }

            header << "Server: Srv2/0.1\r\n";
            header << "Date: " << timestamp() << "\r\n";
            //header << "Connection: close\r\n";
            header << "Content-Length: " << size << "\r\n";
            header << "\r\n";

            response.append(header.str());
            response.append(body);


    //asio::async_write(socket, reply.to_buffers(), func);
    asio::async_write(socket, asio::buffer(response), func);
}

} // namespace server
