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
#include <string>

#include <boost/asio.hpp>

#include "logger.hpp"
#include "keymap.hpp"
#include "server.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
    try {

        auto config = std::make_shared<srv::config>();
        config->address = "0.0.0.0";
        config->port = "1026";
        config->pubdir = "./public";
        config->poolsize = 5;
        config->crtfile = "./server.crt";
        config->keyfile = "./server.key";
        config->backlog = 2048;
        config->index = "index.html";

        auto pwd = std::filesystem::current_path();

        auto filemap = std::make_shared<srv::keymap>();

        filemap->set("html", "text/html");
        filemap->set("js", "application/javascript");
        filemap->set("json", "application/json");
        filemap->set("jpg", "image/jpeg");
        filemap->set("png", "image/png");
        filemap->set("css", "text/css");
        filemap->set("cpp", "text/plain");
        filemap->set("ico", "image/x-icon");

        auto logger = std::make_shared<srv::logger>("./std.log", 3);


        auto ptrbox = std::make_shared<srv::ptrbox>();
        ptrbox->config = config;
        ptrbox->filemap = filemap;
        ptrbox->logger = logger;

        srv::server server(ptrbox);
        server.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }
    return 0;
}
