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

#ifndef SRV_SERVER_HPP
#define SRV_SERVER_HPP

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "connect.hpp"
#include "config.hpp"

namespace srv {

class server {
  private:
    std::shared_ptr<srv::cover> cover;

    boost::asio::io_context io_context;
    boost::asio::signal_set signals;
    boost::asio::ip::tcp::acceptor acceptor;

    boost::asio::ssl::context ssl_context;
    std::shared_ptr<srv::connect> connect;

    void accept();
    void stop();

  public:
    server(std::shared_ptr<srv::cover> cover);

    void run();

    server(const server&) = delete;
    server& operator=(const server&) = delete;

};

} // namespace srv

#endif // SERVER_SERVER_HPP
