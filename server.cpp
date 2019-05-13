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

server::server(std::shared_ptr<srv::cover> cover) :
        cover(cover),
        io_context(),
        signals(io_context),
        acceptor(io_context),
        ssl_context(boost::asio::ssl::context::sslv23),
        connect()
{

    auto address = cover->config->str_value("address");
    auto port = cover->config->str_value("port");
    auto backlog = cover->config->num_value("backlog");
    auto crtfile = cover->config->str_value("crtfile");
    auto keyfile = cover->config->str_value("keyfile");

    signals.add(SIGINT);
    signals.add(SIGTERM);
    auto signal_handler = [this](boost::system::error_code ec, int signo) {
        stop();
    };
    signals.async_wait(signal_handler);


    ssl_context.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::no_sslv3
        | boost::asio::ssl::context::single_dh_use
    );
    ssl_context.use_certificate_chain_file(crtfile);
    ssl_context.use_private_key_file(keyfile, boost::asio::ssl::context::pem);

    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();

    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

    acceptor.bind(endpoint);
    acceptor.listen(backlog);
    accept();
}

void server::run() {
    auto pool_size = cover->config->num_value("threads");
    std::vector<std::shared_ptr<boost::thread>> threads;
    for (std::size_t i = 0; i < pool_size; ++i) {
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
    auto callback = [this](const boost::system::error_code& error) {
        if (!error) {
            connect->start();
        }
        accept();
    };
    connect.reset(new srv::connect(cover, ssl_context, io_context));
    acceptor.async_accept(connect->get_socket(), callback);
}

void server::stop() {
    io_context.stop();
}

} // namespace srv
