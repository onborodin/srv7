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

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "connection.hpp"
#include "request.hpp"
#include "response.hpp"
#include "keymap.hpp"
#include "handler.hpp"

namespace srv {

connection::connection(
        boost::asio::ssl::context& ssl_context,
        boost::asio::io_context& io_context,
        srv::factory& factory) :
            strand(io_context),
            socket(io_context),
            //io_context(io_context),
            ssl_context(ssl_context),
            //factory(factory),
            handler(factory) {
    }

boost::asio::ip::tcp::socket& connection::get_socket() {
    return socket;
}

void connection::start() {
    using sslsocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
    ssl_socket = std::make_shared<sslsocket>(std::move(socket), ssl_context);
    ssl_handshake();
}
void connection::ssl_handshake() {
        auto self(shared_from_this());
        auto callback = [this, self](const boost::system::error_code& error) {
            if (!error) {
                read_header();
            } else {
                stop();
            }
        };
        ssl_socket->async_handshake(boost::asio::ssl::stream_base::server, callback);
}

void connection::read_header() {
    auto self(shared_from_this());
    auto callback = [this, self](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
            request.allocate(request_buffer);
            auto promise_clen = request.content_length();
            auto actual_clen = request.content().length();

            if (promise_clen > 0 && promise_clen > actual_clen) {
                read_content();
            } else {
                handler.route(request, response);
                write();
            }
        } else if (ec != boost::asio::error::operation_aborted) {
                stop();
        }
    };
    std::string delimeter = "\r\n\r\n";
    boost::asio::async_read_until(*ssl_socket, boost::asio::dynamic_buffer(request_buffer), delimeter, callback);
}

void connection::read_content() {
   auto self(shared_from_this());
    auto callback = [this, self](boost::system::error_code ec, std::size_t size) {
        if (!ec) {
            handler.route(request, response);
            write();
        } else if (ec != boost::asio::error::operation_aborted) {
            stop();
        }
    };

    auto promise_len = request.content_length();
    auto actual_len = request.content().length();
    int demaind = promise_len - actual_len;
    request_buffer.clear();
    boost::asio::async_read(*ssl_socket, boost::asio::dynamic_buffer(request_buffer, demaind), callback);
}

void connection::write() {
    auto self(shared_from_this());
    auto cb = [this, self](boost::system::error_code error, std::size_t) {
        if (!error) {
            stop();
        }
        if (error != boost::asio::error::operation_aborted) {
            stop();
        }
    };
    response_buffer = response.str();
    boost::asio::async_write(*ssl_socket, boost::asio::dynamic_buffer(response_buffer), boost::asio::bind_executor(strand, cb));
}

void connection::stop() {
    boost::system::error_code ignored_ec;
    ssl_socket->shutdown(ignored_ec);

}

} // namespace srv
