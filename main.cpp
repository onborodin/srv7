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
#include <string>
#include <string>
#include <filesystem>

#include <asio.hpp>

#include "server.hpp"
#include "config.hpp"
#include "keymap.hpp"

int main(int argc, char* argv[]) {
    try {

        auto config = std::make_shared<srv6::config>();

        config->filemap.set("html", "text/html");
        config->filemap.set("js", "application/javascript");
        config->filemap.set("json", "application/json");
        config->filemap.set("jpg", "image/jpeg");
        config->filemap.set("png", "image/png");
        config->filemap.set("css", "text/css");
        config->filemap.set("cpp", "text/plain");

        config->basedir = std::filesystem::current_path();
        config->basedir += "/public/";

        config->port = "1026";
        config->threads = 5;

        srv6::server s(config);
        s.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
