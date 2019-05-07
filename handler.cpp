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
#include <filesystem>

#include "request.hpp"
#include "utils.hpp"

#include "handler.hpp"
#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include "config.hpp"

namespace srv {

handler::handler(std::shared_ptr<srv::ptrbox> ptrbox) : ptrbox(ptrbox) {}

void handler::handle(http::request& request, http::response& response) {

    auto config = ptrbox->config;
    auto logger = ptrbox->logger;

    std::string method = request.method();
    std::string resource = request.resource();
    if (resource == "/") resource = "/" + config->index;
    std::string path = config->pubdir + "/" + resource;

    std::filesystem::path cpath;
    try {
        cpath = std::filesystem::canonical(std::filesystem::path(path));
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        cpath = std::filesystem::path(path);
    }

    int size = 0;
    std::string content;
    content.clear();
    std::ifstream is(cpath.string(), std::ios::binary | std::ios::in);

    if (is) {
        ////is.seekg(0, std::ios::end);
        ////size = is.tellg();
        ////is.seekg(0, std::ios::beg);
        char buffer[1024];
        while (is.read(buffer, sizeof(buffer)).gcount() > 0) {
            content.append(buffer, is.gcount());
            size += is.gcount();
        }
    }

    std::string content_type;
    auto ext = path.substr(path.find_last_of(".") + 1);
    if (ext == path) ext = "";

    if (ext.length() > 0) {
        content_type = ptrbox->filemap->get(ext);
    }
    if (content_type.length() == 0) {
        content_type = "application/octet-stream";
    }

    std::stringstream response_header_ss;
    std::stringstream log_ss;

    if (size > 0) {
        response.code("200 OK");
        response.header("content-type", content_type);
        response.header("accept-ranges", "bytes");
    } else {
        response.code("404 Not Found");
        response.header("content-type", "text/plain");
        response.header("connection", "close");
    }
    response.content(content);

}

} // namespace srv
