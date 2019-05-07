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

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <regex>

namespace http {

class request {
    private:
        std::unordered_map<std::string, std::string> headers;
        std::string _method;
        std::string _resource;
        std::string _version;
        std::string _content;
    public:
        request();

        void allocate(std::string source);

        std::string method();
        std::string resource();
        std::unordered_map<std::string, std::string> args();

        int content_length();

        bool accept_encoding(std::string enc);
        std::vector<std::string> accept_encoding();

        bool accept(std::string mime);
        std::vector<std::string> accept();

        std::vector<std::string> authorization();

        std::string str();

        void content(std::string& source);
        void content_append(std::string& source);
        std::string& content();
};


} // namespace http

#endif
