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


#ifndef SERVER_CONNECTION_HPP
#define SERVER_CONNECTION_HPP

#include <memory>
#include <array>

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace server {


class connection : public std::enable_shared_from_this<connection> {
  private:
    asio::io_context::strand strand;
    asio::ip::tcp::socket socket;
    asio::ssl::context& ssl_context;

    asio::ssl::stream<asio::ip::tcp::socket> *ssl_socket;

    std::string response;
    std::string request;

    void handshake();
    void read();
    void write();

  public:
    explicit connection(
        asio::ssl::context& ssl_context,
        asio::io_context& io_context
    );
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    asio::ip::tcp::socket& get_socket();

    void start();
    void stop();
    ~connection();
};

} // namespace server

#endif // SERVER_CONNECTION_HPP
