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


#include <utility>
#include <signal.h>

#include "server.hpp"

namespace srv6 {

server::server(std::shared_ptr<srv6::config> config)
    : io_context(1),
        signals(io_context),
        acceptor(io_context),
        config(config),
        manager()
    {
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
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(config->address, config->port).begin();

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
    auto func = [this](std::error_code ec, asio::ip::tcp::socket socket) {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor.is_open()) {
            return;
        }
        if (!ec) {
            manager.start(
                std::make_shared<connection>(std::move(socket), manager, config)
            );
        }
        do_accept();
    };
    acceptor.async_accept(func);
}

void server::do_await_stop() {
    signals.async_wait(
    [this](std::error_code ec, int signo) {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        acceptor.close();
        manager.stop_all();
    });
}

} // namespace srv6
