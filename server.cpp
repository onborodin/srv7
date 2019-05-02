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


#include <asio.hpp>
#include <asio/ssl.hpp>

#include "server.hpp"
#include "config.hpp"

namespace srv {

server::server(std::shared_ptr<srv::ptrbox> ptrbox)
    : signals(io_context),
      acceptor(io_context),
      ssl_context(asio::ssl::context::sslv23),
      connection(),
      ptrbox(ptrbox)
    {

    signals.add(SIGINT);
    signals.add(SIGTERM);

    auto signal_handler = [this](asio::error_code ec, int signo) {
        stop();
    };

    signals.async_wait(signal_handler);

    ssl_context.set_options(
        asio::ssl::context::default_workarounds
        | asio::ssl::context::no_sslv2
        | asio::ssl::context::single_dh_use
    );
    ssl_context.use_certificate_chain_file(ptrbox->config->crtfile);
    ssl_context.use_private_key_file(ptrbox->config->keyfile, asio::ssl::context::pem);

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(
        ptrbox->config->address,
        ptrbox->config->port
    ).begin();

    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(ptrbox->config->backlog);

    this->accept();
}

void server::run() {
    std::vector<std::shared_ptr<asio::thread>> threads;
    for (std::size_t i = 0; i < ptrbox->config->poolsize; ++i) {

        auto f = [this]{ io_context.run(); };
        auto t = new asio::thread(std::move(f));

        std::shared_ptr<asio::thread> thread(t);
        threads.push_back(thread);
    }
    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}

void server::accept() {
    connection.reset(new class connection(ssl_context, io_context, ptrbox));
    auto handler = [this](const asio::error_code& ec) {
        if (!ec) {
            connection->start();
        }
        this->accept();
    };
    acceptor.async_accept(connection->get_socket(), handler);
}

void server::stop() {
    io_context.stop();
}

} // namespace srv
