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

#include "server.hpp"
#include <vector>
#include <memory>

namespace server3 {

server::server(const std::string& address, const std::string& port,
    const std::string& doc_root, std::size_t thread_pool_size)
    : thread_pool_size(thread_pool_size),
      signals(io_context),
      acceptor(io_context),
      new_connection(),
      request_handler(doc_root) {
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.

    signals.add(SIGINT);
    signals.add(SIGTERM);

    auto signal_handler = [this](std::error_code ec, int signo) {
        handle_stop();
    };

    //signals.async_wait(boost::bind(&server::handle_stop, this));
    signals.async_wait(signal_handler);


    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(2048);

    start_accept();
}

void server::run() {

    std::vector<std::shared_ptr<asio::thread>> threads;
    for (std::size_t i = 0; i < thread_pool_size; ++i) {

        //std::shared_ptr<asio::thread> thread(
            //new asio::thread(
                //boost::bind(&asio::io_context::run, &io_context))
        //);

        auto f = [this]{ io_context.run(); };
        auto t = new asio::thread(std::move(f));
        std::shared_ptr<asio::thread> thread(std::move(t));

        threads.push_back(thread);
    }
    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}

void server::start_accept() {
    new_connection.reset(new connection(io_context, request_handler));
    auto handle_accept = [this](const asio::error_code& e) {
        if (!e) {
            new_connection->start();
        }
        start_accept();
    };
    acceptor.async_accept(new_connection->get_socket(), handle_accept);
}

void server::handle_stop() {
    io_context.stop();
}

} // namespace server3
