//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <array>
#include <memory>

#include <asio.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace server {

class connection_manager;

class connection : public std::enable_shared_from_this<connection> {
  private:

    asio::ip::tcp::socket socket;
    class connection_manager& connection_manager;
    class request_handler& request_handler;
    std::array<char, 8192> buffer;
    request request;
    request_parser request_parser;
    reply reply;

    void do_read();
    void do_write();


  public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    explicit connection(asio::ip::tcp::socket socket, class connection_manager& manager, class request_handler& handler);

    void start();
    void stop();
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server

#endif // HTTP_CONNECTION_HPP
