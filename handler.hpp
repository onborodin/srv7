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

namespace srv {

class handler {
    private:
        http::request::header request_header;
        std::shared_ptr<srv::ptrbox> ptrbox;

    public:
        handler(std::string& request_header, std::shared_ptr<srv::ptrbox> ptrbox)
        : request_header(request_header), ptrbox(ptrbox) {}

        void run(std::string& response_header, std::string& response_content) {
            //srv::keymap filemap;
            //filemap.set("html", "text/html");
            //filemap.set("js", "application/javascript");
            //filemap.set("json", "application/json");
            //filemap.set("jpg", "image/jpeg");
            //filemap.set("png", "image/png");
            //filemap.set("css", "text/css");
            //filemap.set("cpp", "text/plain");

            auto config = ptrbox->config;
            auto logger = ptrbox->logger;

            std::string resource = request_header.resource();
            if (resource == "/") resource = "/" + config->index;
            std::string path = config->pubdir + "/" + resource;

            std::filesystem::path cpath;
            try {
                cpath = std::filesystem::canonical(std::filesystem::path(path));
            } catch (std::exception& e) {
                std::cerr << "exception: " << e.what() << "\n";
                cpath = std::filesystem::path(path);
            }

            logger->log(cpath.string());
            //std::cout << cpath.string() << std::endl;

            int size = 0;
            response_content.clear();
            std::ifstream is(cpath.string(), std::ios::binary | std::ios::in);

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
                mime = ptrbox->filemap->get(ext);
            }
            if (mime.length() == 0) {
                mime = "application/octet-stream";
            }

            std::stringstream response_header_ss;

            if (size > 0) {
                response_header_ss << "HTTP/1.1 200 OK\r\n";
                response_header_ss << "Content-Type: " <<  mime << "\r\n";
                response_header_ss << "Content-Length: " << size << "\r\n";
                response_header_ss << "Accept-Ranges: bytes\r\n";

            } else {
                response_header_ss << "HTTP/1.1 404 Not Found\r\n";
                response_header_ss << "Content-Type: text/plain\r\n";
                response_header_ss << "Connection: close\r\n";
            }

            response_header_ss << "Server: Srv3/0.1\r\n";
            response_header_ss << "Date: " << http::utils::timestamp() << "\r\n";
            response_header_ss << "\r\n";

            response_header = response_header_ss.str();
        }
};

} // namespace srv
