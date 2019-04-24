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
#include "manager.hpp"
#include "dispatcher.cpp"
#include "config.hpp"

namespace srv6 {

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

connection::connection(
        asio::ip::tcp::socket socket,
        std::shared_ptr<srv6::manager> manager,
        std::shared_ptr<srv6::config> config
    ) :
    socket(std::move(socket)),
    manager(manager),
    config(config)
    { }

void connection::start() {
    read();
}

void connection::stop() {
    socket.close();
}

void connection::read() {
    auto self(shared_from_this());

    auto func = [this, self](std::error_code ec, std::size_t size) {
        if (!ec) {

            raw_request_header = buffer.substr(0, size);
            buffer.erase(0, size);

            write();
        } else if (ec != asio::error::operation_aborted) {
            manager->stop(shared_from_this());
        }
    };

    std::string delimeter = "\r\n\r\n";
    asio::async_read_until(socket, asio::dynamic_buffer(buffer), delimeter, func);
}

void connection::write() {
    auto self(shared_from_this());
    auto func = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            // initiate graceful connection closure.
            asio::error_code ignored_ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                ignored_ec);
        }
        if (ec != asio::error::operation_aborted) {
            manager->stop(shared_from_this());
        }
    };

    dispatcher dsp(config);
    dsp.parse(raw_request_header);
    dsp.handle(response);

    asio::async_write(socket, asio::buffer(response), func);
}

} // namespace srv6
