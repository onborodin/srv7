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
#include <fstream>
#include <vector>
#include <regex>

#include "config.hpp"

namespace srv {

void config::set_default(std::string key, int value) {
    auto res = num_map.find(key);
    if (res == num_map.end()) {
        std::pair<std::string, int> pair = { key, value };
        num_map.insert(pair);
    }
}

void config::set_default(std::string key, std::string value) {
    auto res = str_map.find(key);
    if (res == str_map.end()) {
        std::pair<std::string, std::string> pair = { key, value };
        str_map.insert(pair);
    }
}

bool config::read(std::string filename) {
    std::ifstream is(filename, std::ios::binary | std::ios::in);
    if (!is) {
        return false;
    }
    std::string tmp;
    char buffer[1024];
    while (is.read(buffer, sizeof(buffer)).gcount() > 0) {
        tmp.append(buffer, is.gcount());
    }
    std::string content = std::regex_replace(tmp, std::regex("\r\n"), "\n");
    auto lines = split(content, "\n");
    for (auto& line: lines) {
        std::smatch match;
        std::regex_search(line, match, std::regex("^[[:blank:]]*([a-zA-Z0-9_]+)[[:blank:]]*[:=][[:blank:]]*([a-z_A-Z0-9]+)[[:blank:]]*[;]+"));
        if (match.size() == 3) {
            auto key = match[1];
            auto value = match[2];
            std::pair<std::string, std::string> pair = { key, value };
            str_map.erase(key);
            str_map.insert(pair);
        }
    }
    for (auto& line: lines) {
        std::smatch match;
        std::regex_search(line, match, std::regex("^[[:blank:]]*([a-zA-Z0-9]+)[[:blank:]]*=[[:blank:]]*([0-9]+)[[:blank:]]*[;]+"));
        if (match.size() == 3) {
            auto key = match[1];
            auto value = std::atoi(std::string(match[2]).c_str());
            std::pair<std::string, int> pair = { key, value };
            num_map.erase(key);
            num_map.insert(pair);
        }
    }
    return true;
}

int config::num_value(const std::string& key) {
        auto value = num_map.find(key);
        if (value != num_map.end()) {
            return value->second;
        }
        return -1;
}

std::string config::str_value(const std::string& key) {
        auto value = str_map.find(key);
        if (value != str_map.end()) {
            return std::string(value->second);
        }
        return std::string("");
}

std::vector<std::string> config::split(const std::string& source, const std::string& delimiter) {
    std::vector<std::string> outv;
    std::size_t begin = 0;
    std::size_t position = std::string::npos;
    while ((position = source.find(delimiter, begin)) != std::string::npos) {
        std::string substring = source.substr(begin, position - begin);
        begin = position + delimiter.size();
        outv.push_back(substring);
    }
    std::string substring = source.substr(begin);
    outv.push_back(substring);
    return outv;
}

} // namespce srv
