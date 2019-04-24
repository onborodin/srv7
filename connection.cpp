/*
 *
 * Copyright 2019 Oleg Borodin  <borodin@unix7.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

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

#include "connection.hpp"
#include "connection_manager.hpp"

namespace server {

connection::connection(asio::ip::tcp::socket socket, class connection_manager& manager)
    : socket(std::move(socket)), connection_manager(manager)
    { }

void connection::start() {
    do_read();
}

void connection::stop() {
    socket.close();
}

void connection::do_read() {
    auto self(shared_from_this());

    auto func = [this, self](std::error_code ec, std::size_t size) {
        if (!ec) {

            std::string header = buffer.substr(0, size - 4);
            buffer.erase(0, size);
            //std::cerr << header << std::endl;

            do_write();
        } else if (ec != asio::error::operation_aborted) {
            connection_manager.stop(shared_from_this());
        }
    };

    std::string delimeter = "\r\n\r\n";
    asio::async_read_until(socket, asio::dynamic_buffer(buffer), delimeter, func);

    //std::string buffer;
    //socket.async_read_some(asio::buffer(buffer), func);
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
