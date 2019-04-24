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

#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>

#include "manager.hpp"
#include "config.hpp"
#include "connection.hpp"

#include <asio.hpp>

namespace srv6 {

class server {
  private:
    asio::io_context io_context;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;

    std::shared_ptr<srv6::config> config;
    std::shared_ptr<srv6::manager> manager;

    void do_accept();
    void do_await_stop();

  public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(
        std::shared_ptr<srv6::config> config,
        std::shared_ptr<srv6::manager> manager
    );
    void run();

};

} // namespace srv6

#endif
