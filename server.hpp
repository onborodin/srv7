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


#ifndef SRV7_SERVER_HPP
#define SRV7_SERVER_HPP

#include <string>
#include <vector>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "connection.hpp"

namespace srv7 {

class server {
  private:
    std::size_t thread_pool_size;
    asio::io_context io_context;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;
    asio::ssl::context ssl_context;

    std::shared_ptr<connection> connection;

    void accept();
    void stop();

  public:
    explicit server(
        const std::string& address,
        const std::string& port,
        std::size_t thread_pool_size
    );

    void run();

    server(const server&) = delete;
    server& operator=(const server&) = delete;

};

} // namespace srv7

#endif // SRV7_SERVER_HPP
