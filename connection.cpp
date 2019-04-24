//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include <vector>
//#include <boost/bind.hpp>
#include "request_handler.hpp"

namespace http {
namespace server3 {

connection::connection(asio::io_context& io_context,
    class request_handler& handler)
    : strand(io_context),
      socket(io_context),
      request_handler(handler) {
}

asio::ip::tcp::socket& connection::get_socket() {
    return socket;
}


void connection::start() {
  do_read();
}

void connection::do_read() {
    auto self(shared_from_this());

    auto handler = [this, self](std::error_code ec, std::size_t bytes_transferred) {
        if (!ec) {
            boost::tribool result;
            boost::tie(result, boost::tuples::ignore) = request_parser.parse(
                request, buffer.data(), buffer.data() + bytes_transferred
            );
            if (result) {
                request_handler.handle_request(request, reply);
                do_write();
            } else if (!result) {
                reply = reply::stock_reply(reply::bad_request);
                do_write();
            } else {
                do_read();
            }
        } else if (ec != asio::error::operation_aborted) {
            ////connection_manager_.stop(shared_from_this());
        }
    };

    socket.async_read_some(asio::buffer(buffer), handler);
}

void connection::do_write() {
    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            asio::error_code ignored_ec;
            socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
        if (ec != asio::error::operation_aborted) {
            //connection_manager_.stop(shared_from_this());
        }
    };
    asio::async_write(socket, reply.to_buffers(), asio::bind_executor(strand, handler));
}

//void connection::start() {
    //socket.async_read_some(asio::buffer(buffer),
        //asio::bind_executor(strand,
            //boost::bind(&connection::handle_read, shared_from_this(),
                //asio::placeholders::error,
                //asio::placeholders::bytes_transferred)));
//}

//void connection::handle_read(const asio::error_code& e,
    //std::size_t bytes_transferred) {
    //if (!e) {
        //boost::tribool result;
        //boost::tie(result, boost::tuples::ignore) = request_parser.parse(
                //request, buffer.data(), buffer.data() + bytes_transferred);

        //if (result) {
            //request_handler.handle_request(request, reply);
            //asio::async_write(socket, reply.to_buffers(),
                //asio::bind_executor(strand,
                    //boost::bind(&connection::handle_write, shared_from_this(),
                        //asio::placeholders::error)));
        //} else if (!result) {
            //reply = reply::stock_reply(reply::bad_request);
            //asio::async_write(socket, reply.to_buffers(),
                //asio::bind_executor(strand,
                    //boost::bind(&connection::handle_write, shared_from_this(),
                        //asio::placeholders::error)));
        //} else {
            //socket.async_read_some(asio::buffer(buffer),
                //asio::bind_executor(strand,
                    //boost::bind(&connection::handle_read, shared_from_this(),
                        //asio::placeholders::error,
                        //asio::placeholders::bytes_transferred)));
        //}
    //}

     //If an error occurs then no new asynchronous operations are started. This
     //means that all shared_ptr references to the connection object will
     //disappear and the object will be destroyed automatically after this
     //handler returns. The connection class's destructor closes the socket.
//}

//void connection::handle_write(const asio::error_code& e) {
    //if (!e) {
        //// Initiate graceful connection closure.
        //asio::error_code ignored_ec;
        //socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
    //}

     //No new asynchronous operations are started. This means that all shared_ptr
     //references to the connection object will disappear and the object will be
     //destroyed automatically after this handler returns. The connection class's
     //destructor closes the socket.
//}

} // namespace server3
} // namespace http
