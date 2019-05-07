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


#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <regex>

namespace http {
namespace utils {

std::string base64_encode(const std::string &in);
std::string base64_decode(const std::string &in);
std::vector<std::string> split(const std::string& s, const std::string& delimiter);
std::string tolower(std::string s);
std::string toupper(std::string s);
std::string camelcase (const std::string& str, const std::string delimiter = "-");
std::string timestamp(std::time_t shift = 0);
std::string unixtime(std::time_t shift = 0);
bool match(const std::string& string, const std::string& pattern);

} // namespace utils
} // namespace http

#endif
