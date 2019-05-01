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

#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <iomanip>
#include <chrono>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "connection.hpp"
#include "request.hpp"
#include "keymap.hpp"

namespace server {

connection::connection(asio::ssl::context& ssl_context, asio::io_context& io_context)
    : strand(io_context), socket(io_context), ssl_context(ssl_context) {}

asio::ip::tcp::socket& connection::get_socket() {
    return socket;
}

void connection::start() {
    ssl_socket = new asio::ssl::stream<asio::ip::tcp::socket>(
        std::move(socket),
        ssl_context
    );
    handshake();
}



void connection::handshake() {
        auto self(shared_from_this());
        ssl_socket->async_handshake(asio::ssl::stream_base::server,
        [this, self](const std::error_code& error) {
            if (!error) {
                this->read();
            } else {
                this->stop();
            }
        });
}

void connection::read() {
    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t size) {
        if (!ec) {
            this->write();
        } else if (ec != asio::error::operation_aborted) {
            this->stop();
        }
    };
    std::string delimeter = "\r\n\r\n";
    asio::async_read_until(
        *ssl_socket, asio::dynamic_buffer(request),
        delimeter,
        handler
    );
}

std::string timestamp() {
    std::stringstream ss;
    std::time_t now = std::time(0);
    ss << std::put_time(std::gmtime(&now), "%a, %d %b %Y %T %Z");
    return ss.str();
}

void connection::write() {

    //std::stringstream response_content;
    //for (int i = 0; i < 10000; i++) {
        //response_content << "hello!";
    //}

    //std::stringstream response_header;
    //response_header << "HTTP/1.1 200 OK\r\n";
    //response_header << "Date: " << timestamp() << "\r\n";
    //response_header << "Server: Srv3/0.1\r\n";
    //response_header << "Content-Length: " << response_content.str().length() << "\r\n";
    //response_header << "Content-Type: text/plain\r\n";
    //response_header << "Connection: close\r\n";
    //response_header << "\r\n";

    //response += response_header.str();
    //response += response_content.str();

    server::keymap filemap;

    filemap.set("html", "text/html");
    filemap.set("js", "application/javascript");
    filemap.set("json", "application/json");
    filemap.set("jpg", "image/jpeg");
    filemap.set("png", "image/png");
    filemap.set("css", "text/css");
    filemap.set("cpp", "text/plain");

    http::request::header request_header(request);


    std::string resource = request_header.resource();
    if (resource == "/") resource = "/index.html";
    std::string path = "./public/" + resource;

    std::string response_content{""};

    int size = 0;
    response_content.clear();
    std::ifstream is(path, std::ios::binary | std::ios::in);

    if (is) {
        //is.seekg(0, std::ios::end);
        //size = is.tellg();
        //is.seekg(0, std::ios::beg);
        char buffer[1024];
        while (is.read(buffer, sizeof(buffer)).gcount() > 0) {
            response_content.append(buffer, is.gcount());
            size += is.gcount();
        }
    }

    std::string mime;
    auto ext = path.substr(path.find_last_of(".") + 1);
    if (ext == path) ext = "";

    if (ext.length() > 0) {
        mime = filemap.get(ext);
    }
    if (mime.length() == 0) {
        mime = "application/octet-stream";
    }

    std::stringstream response_header;

    if (size > 0) {
        response_header << "HTTP/1.1 200 OK\r\n";
        response_header << "Content-Type: " <<  mime << "\r\n";
        response_header << "Content-Length: " << size << "\r\n";
        response_header << "Accept-Ranges: bytes\r\n";

    } else {
        response_header << "HTTP/1.1 404 Not Found\r\n";
        response_header << "Content-Type: text/plain\r\n";
        response_header << "Connection: close\r\n";
    }

    response_header << "Server: Srv3/0.1\r\n";
    response_header << "Date: " << timestamp() << "\r\n";
    response_header << "\r\n";


    response.append(response_header.str());
    response.append(response_content);

    auto self(shared_from_this());
    auto handler = [this, self](std::error_code ec, std::size_t) {
        if (!ec) {
            this->stop();
        }
        if (ec != asio::error::operation_aborted) {
            this->stop();
        }
    };
    asio::async_write(
        *ssl_socket, asio::buffer(response),
        asio::bind_executor(strand, handler)
    );
}

void connection::stop() {
    asio::error_code ignored_ec;
    ssl_socket->shutdown(ignored_ec);
}

connection::~connection() {
    delete ssl_socket;
}

} // namespace server
