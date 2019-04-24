//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_SERVER_HPP
#define HTTP_SERVER3_SERVER_HPP

#include <string>
#include <vector>

#include <asio.hpp>

//#include <boost/noncopyable.hpp>
//#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "request_handler.hpp"

namespace http {
namespace server3 {

/// The top-level class of the HTTP server.
class server {
  public:
    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit server(const std::string& address, const std::string& port,
        const std::string& doc_root, std::size_t thread_pool_size);

    /// Run the server's io_context loop.
    void run();

    server(const server&) = delete;
    server& operator=(const server&) = delete;

  private:
    /// Initiate an asynchronous accept operation.
    void start_accept();

    /// Handle completion of an asynchronous accept operation.
    //void handle_accept(const asio::error_code& e);

    /// Handle a request to stop the server.
    void handle_stop();

    /// The number of threads that will call io_context::run().
    std::size_t thread_pool_size;

    /// The io_context used to perform asynchronous operations.
    asio::io_context io_context;

    /// The signal_set is used to register for process termination notifications.
    asio::signal_set signals;

    /// Acceptor used to listen for incoming connections.
    asio::ip::tcp::acceptor acceptor;

    /// The next connection to be accepted.
    connection_ptr new_connection;

    /// The handler for all incoming requests.
    request_handler request_handler;
};

} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_SERVER_HPP
