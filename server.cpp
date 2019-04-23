//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <signal.h>
#include <utility>

namespace server {

server::server(const std::string& address, const std::string& port, const std::string& doc_root)
    : io_context(1),
      signals(io_context),
      acceptor(io_context),
      connection_manager(),
      request_handler(doc_root) {
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals.add(SIGINT);
    signals.add(SIGTERM);
    #if defined(SIGQUIT)
    signals.add(SIGQUIT);
    #endif // defined(SIGQUIT)

    do_await_stop();

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();

    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(2048);

    do_accept();
}

void server::run() {
    io_context.run();
}

void server::do_accept() {
    acceptor.async_accept(
    [this](std::error_code ec, asio::ip::tcp::socket socket) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor.is_open()) {
            return;
        }
        if (!ec) {
            connection_manager.start(
                std::make_shared<connection>(std::move(socket), connection_manager, request_handler)
            );
        }
        do_accept();
    });
}

void server::do_await_stop() {
    signals.async_wait(
    [this](std::error_code ec, int signo) {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        acceptor.close();
        connection_manager.stop_all();
    });
}

} // namespace server
