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
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <regex>

#include "utils.hpp"
#include "response.hpp"

namespace http {

//namespace code {
//const std::string ok = "200 OK";
//const std::string created = "201 Created";
//const std::string accepted = "202 Accepted";
//const std::string no_content = "204 No Content";
//const std::string multiple_choices = "300 Multiple Choices";
//const std::string moved_permanently = "301 Moved Permanently";
//const std::string moved_temporarily = "302 Moved Temporarily";
//const std::string not_modified = "304 Not Modified";
//const std::string bad_request = "400 Bad Request";
//const std::string unauthorized = "401 Unauthorized";
//const std::string forbidden = "403 Forbidden";
//const std::string not_found = "404 Not Found";
//const std::string internal_server_error = "500 Internal Server Error";
//const std::string not_implemented = "501 Not Implemented";
//const std::string bad_gateway = "502 Bad Gateway";
//const std::string service_unavailable = "503 Service Unavailable";
//} // namespace code

void response::header(const std::string& key, const std::string& value) {
    std::pair<std::string, std::string> pair = { http::utils::tolower(key), value };
    headers.insert(pair);
}

void response::code(const std::string& source) {
    _code = source;
}

void response::content(const std::string& source) {
    _content = source;
}

std::string& response::content() {
    return _content;
}


std::string response::str() {
        std:: stringstream header_ss;
        header_ss << "HTTP/1.1 " << _code << "\r\n";
        for(auto& i: headers) {
            header_ss << http::utils::camelcase(i.first) << ": " << i.second << "\r\n";
        }
        if (_content.length() > 0) {
            header_ss << "Content-Length: " << _content.length() << "\r\n";
        }
        header_ss << "Server: Srv7/0.1\r\n";
        header_ss << "Date: " << http::utils::timestamp() << "\r\n";
        header_ss << "\r\n";

        std::string res;
        res.append(header_ss.str());
        res.append(_content);
        return res;
}


} // namespace http
