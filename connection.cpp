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
#include <fstream>
#include <filesystem>
#include <utility>
#include <iomanip>
#include <chrono>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "connection.hpp"
#include "request.hpp"
#include "keymap.hpp"
#include "handler.hpp"

namespace srv {

connection::connection(
        asio::ssl::context& ssl_context,
        asio::io_context& io_context,
        std::shared_ptr<srv::ptrbox> ptrbox
    ) : strand(io_context),
        socket(io_context),
        ssl_context(ssl_context),
        ptrbox(ptrbox)
     {}

asio::ip::tcp::socket& connection::get_socket() {
    return socket;
}

void connection::start() {
    ssl_socket = new asio::ssl::stream<asio::ip::tcp::socket>(
        std::move(socket),
        ssl_context
    );
    handshake();
}


void connection::handshake() {
        auto self(shared_from_this());
        ssl_socket->async_handshake(asio::ssl::stream_base::server,
        [this, self](const asio::error_code& error) {
            if (!error) {
                this->read();
            } else {
                this->stop();
            }
        });
}

void connection::read() {
    auto self(shared_from_this());
    auto handler = [this, self](asio::error_code ec, std::size_t size) {
        if (!ec) {
            this->write();
        } else if (ec != asio::error::operation_aborted) {
            this->stop();
        }
    };
    std::string delimeter = "\r\n\r\n";
    asio::async_read_until(
        *ssl_socket, asio::dynamic_buffer(request),
        delimeter,
        handler
    );
}


void connection::write() {

    srv::handler handler(request, ptrbox);
    std::string response_header;
    std::string response_content;
    handler.run(response_header, response_content);

    response.append(response_header);
    response.append(response_content);

    auto self(shared_from_this());
    auto async_handler = [this, self](asio::error_code ec, std::size_t) {
        if (!ec) {
            this->stop();
        }
        if (ec != asio::error::operation_aborted) {
            this->stop();
        }
    };
    asio::async_write(
        *ssl_socket, asio::buffer(response),
        asio::bind_executor(strand, async_handler)
    );
}

void connection::stop() {
    asio::error_code ignored_ec;
    ssl_socket->shutdown(ignored_ec);
}

connection::~connection() {
    delete ssl_socket;
}

} // namespace srv
