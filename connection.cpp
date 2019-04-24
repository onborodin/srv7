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


#include <vector>
#include <sstream>
#include <filesystem>
#include <utility>
#include <iomanip>
#include <chrono>

#include "connection.hpp"

namespace server3 {

connection::connection(asio::io_context& io_context)
    : strand(io_context), socket(io_context)
    {}

asio::ip::tcp::socket& connection::get_socket() {
    return socket;
}

void connection::start() {
  do_read();
}

void connection::do_read() {
    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            do_write();

        } else if (ec != asio::error::operation_aborted) {
            socket.close();
        }
    };
    std::string delimeter = "\r\n\r\n";
    asio::async_read_until(socket, asio::dynamic_buffer(buffer), delimeter, handler);
}

std::string timestamp() {
    std::stringstream ss;
    std::time_t now = std::time(0);
    ss << std::put_time(std::gmtime(&now), "%a, %d %b %Y %T %Z");
    return ss.str();
}

void connection::do_write() {

    std::stringstream content;
    for (int i = 0; i < 100000; i++) {
        content << "hello!\n";
    }

    std::stringstream header;
    header << "HTTP/1.1 200 OK\r\n";
    header << "Date: " << timestamp() << "\r\n";
    header << "Server: Srv3/0.1\r\n";
    header << "Content-Length: " << content.str().length() << "\r\n";
    header << "Content-Type: text/plain\r\n";
    header << "Connection: close\r\n";
    header << "\r\n";

    response += header.str();
    response += content.str();

    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            asio::error_code ignored_ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
        if (ec != asio::error::operation_aborted) {
            socket.close();
        }
    };
    asio::async_write(socket, asio::buffer(response), asio::bind_executor(strand, handler));
}

} // namespace server3
