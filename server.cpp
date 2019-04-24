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

server::server(const std::string& address, const std::string& port, std::size_t thread_pool_size)
    : thread_pool_size(thread_pool_size),
      signals(io_context),
      acceptor(io_context),
      new_connection()
    {

    signals.add(SIGINT);
    signals.add(SIGTERM);

    auto signal_handler = [this](std::error_code ec, int signo) {
        stop();
    };

    signals.async_wait(signal_handler);

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();

    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(2048);
    accept();
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

void server::accept() {
    new_connection.reset(new connection(io_context));
    auto handler = [this](const asio::error_code& ec) {
        if (!ec) {
            new_connection->start();
        }
        accept();
    };
    acceptor.async_accept(new_connection->get_socket(), handler);
}

void server::stop() {
    io_context.stop();
}

} // namespace server3
