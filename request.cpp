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

#include "request.hpp"
#include "utils.hpp"

namespace http {

request::request() {};
//request(std::string source) { allocate(source) };

void request::allocate(std::string source) {

    std::size_t position = std::string::npos;
    std::string delimiter = "\r\n\r\n";
    std::string header_part;
    std::string content_part;
    if ((position = source.find(delimiter, 0)) != std::string::npos) {
        header_part = source.substr(0, position + delimiter.length());
        content_part = source.substr(position + delimiter.length(), source.length());
    } else {
        header_part = source;
        content_part = "";
    }

    _content = content_part;

    auto headerv = http::utils::split(header_part, "\r\n");
    auto line = headerv.front();

    std::smatch match;
    std::regex_search(line, match, std::regex("^(GET|POST|HEAD)[ ]+(.*)[ ]+(HTTP/1.[01])$"));

    if (match.size() == 4) {
        _method = http::utils::toupper(match[1]);
        _resource = match[2];
        _version = http::utils::toupper(match[3]);
    }

    for(auto line: headerv) {
        std::smatch match;
        std::regex_search(line, match, std::regex("^(.*): (.*)$"));
        if (match.size() == 3 && match[1].length() > 0 && match[2].length() > 0) {
            auto key = http::utils::tolower(match[1]);
            auto value = match[2];
            headers.erase(key);
            std::pair<std::string, std::string> pair = { key, value };
            headers.insert(pair);
        }
    }
}

std::string request::method() {
    return _method;
}

std::string request::resource() {
    return http::utils::split(_resource, "?").at(0);
}

std::unordered_map<std::string, std::string> request::args() {
    auto argstr = http::utils::split(_resource, "?").at(1);
    std::unordered_map<std::string,std::string> result;
    if (argstr.length() > 0) {
        auto argv = http::utils::split(argstr, "&");
        for(auto& i: argv) {
            auto keyvalue = http::utils::split(i, "=");
            //result.erase(keyvalue[0]);
            result.insert(std::make_pair(keyvalue[0], keyvalue[1]));
        }
    }
    return result;
}

int request::content_length() {
    auto value = headers.find("content-length");
    if (value != headers.end()) {
        return std::stoi(value->second);
    }
    return 0;
}

bool request::accept_encoding(std::string enc) {
    auto res = headers.find("accept-encoding");
    if (res != headers.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        auto v = http::utils::split(line, ",");
        for(auto i: v) {
            if (i == enc) return true;
        }
    }
    return false;
}

std::vector<std::string> request::accept_encoding() {
    std::vector<std::string> result;
    auto res = headers.find("accept-encoding");
    if (res != headers.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        result = http::utils::split(line, ",");
    }
    return result;
}

bool request::accept(std::string mime) {
    auto res = headers.find("accept");
    if (res != headers.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        auto v = http::utils::split(line, ",");
        for(auto i: v) {
            if (i == "*/*") return true;
        }
        for(auto i: v) {
            if (i == mime) return true;
        }
    }
    return false;
}

std::vector<std::string> request::accept() {
    std::vector<std::string> result;
    auto res = headers.find("accept");
    if (res != headers.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        result = http::utils::split(line, ",");
    }
    return result;
}

std::vector<std::string> request::authorization() {
    std::vector<std::string> result;
    auto found = headers.find("authorization");
    if (found != headers.end()) {
        std::smatch match;
        std::regex regex("^(basic)[ ]+(.*)$", std::regex_constants::icase);
        std::regex_search(found->second, match, regex);
        if (match[2].length() > 0) {
            auto keyvalue = http::utils::split(http::utils::base64_decode(match[2]), ":");
            if (keyvalue.size() == 2) {
                    result = keyvalue;

            }
        }
    }
    return result;
}

std::string request::str() {
    std::string res;
    res = _method + " " + _resource + " " + _version + "\r\n";
    for(auto& i: headers) {
        res +=  http::utils::camelcase(i.first) + ": " + i.second + "\r\n";
    }
    return res;
}

void request::content(std::string& source) {
    _content = source;
}
void request::content_append(std::string& source) {
    _content += source;
}
std::string& request::content() {
    return _content;
}

} // namespace http
