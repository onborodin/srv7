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


#ifndef HTTP_SERVER3_CONNECTION_HPP
#define HTTP_SERVER3_CONNECTION_HPP

#include <memory>
#include <array>

#include <asio.hpp>

//#include "reply.hpp"
//#include "request.hpp"
//#include "request_handler.hpp"
//#include "request_parser.hpp"

namespace server3 {

class connection : public std::enable_shared_from_this<connection> {

  public:
    explicit connection(asio::io_context& io_context);

    asio::ip::tcp::socket& get_socket();

    void start();
    void do_read();
    void do_write();

    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

  private:
    asio::io_context::strand strand;
    asio::ip::tcp::socket socket;
    std::string response;
    std::string buffer;

    //class request_handler& request_handler;

    //std::array<char, 8192> buffer;

    //class request request;

    //request_parser request_parser;
    //reply reply;
};

} // namespace server3

#endif // HTTP_SERVER3_CONNECTION_HPP
