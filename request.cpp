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
namespace request {

header::header(std::string& source) {
    auto headerv = http::utils::split(source, "\r\n");
    auto line = headerv.front();

    std::smatch match;
    std::regex_search(line, match, std::regex("^(GET|POST|HEAD)[ ]+(.*)[ ]+(HTTP/1.[01])$"));

    if (match.size() == 4) {
        _method = http::utils::tolower(match[1]);
        _resource = match[2];
        _version = http::utils::tolower(match[3]);
    }

    for(auto line: headerv) {
        std::smatch match;
        std::regex_search(line, match, std::regex("^(.*): (.*)$"));
        if (match.size() == 3 && match[1].length() > 0 && match[2].length() > 0) {
            auto key = http::utils::tolower(match[1]);
            auto value = match[2];
            map.erase(key);
            std::pair<std::string, std::string> pair = { key, value };
            map.insert(pair);
        }
    }
}

header::type header::method() {
    if (_method == "get") return type::get;
    if (_method == "post") return type::post;
    if (_method == "head") return type::head;
    return type::unknown;
}

std::string header::resource() {
    return http::utils::split(_resource, "?").at(0);
}

std::unordered_map<std::string, std::string> header::args() {
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


int header::content_length() {
    auto value = map.find("content-length");
    if (value != map.end()) {
        return std::stoi(value->second);
    }
    return 0;
}

bool header::accept_encoding(std::string enc) {
    auto res = map.find("accept-encoding");
    if (res != map.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        auto v = http::utils::split(line, ",");
        for(auto i: v) {
            if (i == enc) return true;
        }
    }
    return false;
}

std::vector<std::string> header::accept_encoding() {
    std::vector<std::string> result;
    auto res = map.find("accept-encoding");
    if (res != map.end()) {
        auto line = std::regex_replace(res->second, std::regex("(,)[ ]+"), "$1");
        result = http::utils::split(line, ",");
    }
    return result;
}

std::vector<std::string> header::authorization() {
    std::vector<std::string> result;
    auto found = map.find("authorization");
    if (found != map.end()) {
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

std::string header::str() {
    std::string res;
    for(auto& i: map) {
        res +=  i.first + ": " + i.second + "\r\n";
    }
    return res;
}

} // namespace http
} // namespace request
