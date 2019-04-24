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

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <memory>

#include <asio.hpp>

#include "request.hpp"
#include "config.hpp"

namespace srv6 {

class manager;

class connection : public std::enable_shared_from_this<connection> {
  private:
    asio::ip::tcp::socket socket;
    //class manager& manager;
    std::shared_ptr<srv6::manager> manager;
    std::shared_ptr<srv6::config> config;

    std::string buffer;
    std::string raw_request_header;
    srv6::request request;

    std::string response;

    void read();
    void write();

  public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    explicit connection(
        asio::ip::tcp::socket socket,
        std::shared_ptr<srv6::manager> manager,
        std::shared_ptr<srv6::config> config
    );

    void start();
    void stop();
};

} // namespace srv6

#endif
