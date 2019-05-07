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

#include <iostream>
#include <vector>
#include <memory>

#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "server.hpp"
#include "config.hpp"

namespace srv {

server::server(srv::factory& factory) :
        io_context(5),
        signals(io_context),
        acceptor(io_context),
        ssl_context(boost::asio::ssl::context::sslv23),
        connection(),
        factory(factory)
    {
    signals.add(SIGINT);
    signals.add(SIGTERM);

    auto signal_handler = [this](boost::system::error_code ec, int signo) {
        stop();
    };

    signals.async_wait(signal_handler);

    ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use
    );
    ssl_context.use_certificate_chain_file(factory.config->crtfile);
    ssl_context.use_private_key_file(factory.config->keyfile, boost::asio::ssl::context::pem);

    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(
        factory.config->address,
        factory.config->port
    ).begin();

    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(factory.config->backlog);

    this->accept();
}

void server::run() {
    std::vector<std::shared_ptr<boost::thread>> threads;
    for (std::size_t i = 0; i < factory.config->poolsize; ++i) {

        auto f = [this]{ io_context.run(); };
        auto t = new boost::thread(std::move(f));

        std::shared_ptr<boost::thread> thread(t);
        threads.push_back(thread);
    }
    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}

void server::accept() {
    boost::asio::ip::tcp::socket socket(io_context);
    //boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket(std::move(socket), ssl_context);

    auto callback = [this](const boost::system::error_code& error) {
        if (!error) {
            connection->start();
        }
        this->accept();
    };
    connection.reset(new class connection(ssl_context, io_context, factory));
    acceptor.async_accept(connection->get_socket(), callback);
}

void server::stop() {
    io_context.stop();
}

} // namespace srv
