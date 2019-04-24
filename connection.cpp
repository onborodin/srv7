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

#include "request_handler.hpp"
#include "connection.hpp"

namespace server3 {

connection::connection(asio::io_context& io_context,
    class request_handler& handler)
    : strand(io_context),
      socket(io_context),
      request_handler(handler) {
}

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
            boost::tribool result;
            boost::tie(result, boost::tuples::ignore) = request_parser.parse(
                request, buffer.data(), buffer.data() + bytes_transferred
            );
            if (result) {
                request_handler.handle_request(request, reply);
                do_write();
            } else if (!result) {
                reply = reply::stock_reply(reply::bad_request);
                do_write();
            } else {
                do_read();
            }
        } else if (ec != asio::error::operation_aborted) {
            ////connection_manager_.stop(shared_from_this());
        }
    };
    socket.async_read_some(asio::buffer(buffer), handler);
}

void connection::do_write() {
    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            asio::error_code ignored_ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
        if (ec != asio::error::operation_aborted) {
            //connection_manager_.stop(shared_from_this());
        }
    };
    asio::async_write(socket, reply.to_buffers(), asio::bind_executor(strand, handler));
}

} // namespace server3
