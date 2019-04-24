//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_CONNECTION_HPP
#define HTTP_SERVER3_CONNECTION_HPP

#include <asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server3 {

/// Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection> {

  public:
    /// Construct a connection with the given io_context.
    explicit connection(asio::io_context& io_context, request_handler& handler);

    asio::ip::tcp::socket& get_socket();

    void start();
    void do_read();
    void do_write();

    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

  private:
    asio::io_context::strand strand;
    asio::ip::tcp::socket socket;

    class request_handler& request_handler;

    boost::array<char, 8192> buffer;

    class request request;

    request_parser request_parser;
    reply reply;
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_CONNECTION_HPP
