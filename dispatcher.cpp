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
#include <fstream>
#include <sstream>
#include <filesystem>

#include "request.hpp"
#include "requestcc.hpp"
#include "config.hpp"
#include "keymap.hpp"

std::string get_ext(std::string name) {
    auto ext = name.substr(name.find_last_of(".") + 1);
    if (ext == name) ext = "";
    return ext;
}


class dispatcher {
    private:
        srv6::request request;
        std::shared_ptr<srv6::config> config;

        std::string timestamp() {
            std::stringstream ss;
            std::time_t now = std::time(0);
            ss << std::put_time(std::gmtime(&now), "%a, %d %b %Y %T %Z");
            return ss.str();
        }

        std::string get_mime(std::string name) {
            auto ext = name.substr(name.find_last_of(".") + 1);
            if (ext == name) ext = "";
            std::string mime;
            if (ext.length() > 0) {
                mime = config->filemap.get(ext);
            }
            if (mime.length() == 0) {
                mime = "application/octet-stream";
            }
            return mime;
        }


    public:
        dispatcher(std::shared_ptr<srv6::config> config) : config(config) {
        };
        void parse(std::string& header) {
            requestcc::compiler(header, request);
        }

        int readfile(std::string& name, std::string& content) {
            std::ifstream is(name, std::ifstream::binary);
            if (is) {
                std::filesystem::path p(name);
                int size = std::filesystem::file_size(p);

                char* buffer = new char[size];
                is.read(buffer, size);
                is.close();
                content = buffer;

                delete[] buffer;
                return size;
              }
            return -1;
        }

        void handle(std::string& res) {
            std::stringstream header;
            std::string body = "";

            if (request.uri == "/") {
                request.uri = "/index.html";
            }

            std::string path = config->publicdir + "/"+ request.uri;

            //std::cerr << path << std::endl;

            int size = 0;
            if ((size = readfile(path, body)) > 0) {
                header << "HTTP/1.1 200 OK\r\n";
                header << "Content-Type: " <<  get_mime(path) << "\r\n";
            } else {
                header << "HTTP/1.1 404 Not Found\r\n";
                header << "Content-Type: text/plain\r\n";
            }

            header << "Server: Srv3/0.1\r\n";
            header << "Date: " << timestamp() << "\r\n";
            header << "Connection: close\r\n";
            header << "Content-Length: " << size << "\r\n";
            header << "\r\n";

            res = "";
            res.append(header.str());
            res.append(body);
        }
};
