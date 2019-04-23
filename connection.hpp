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
  public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// Construct a connection with the given socket.
    explicit connection(asio::ip::tcp::socket socket,
        connection_manager& manager, request_handler& handler);

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

  private:
    void do_read();
    void do_write();

    asio::ip::tcp::socket socket_;
    connection_manager& connection_manager_;

    request_handler& request_handler_;

    std::array<char, 8192> buffer_;

    request request_;

    request_parser request_parser_;

    reply reply_;
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server

#endif // HTTP_CONNECTION_HPP
