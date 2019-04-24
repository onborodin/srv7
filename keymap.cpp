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


#include <mutex>
#include <map>

#include "keymap.hpp"

namespace srv6 {

std::string keymap::tolower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){
            return std::tolower(c);
    });
    return s;
}

void keymap::set(std::string key, std::string arg) {
    key = tolower(key);
    std::pair<std::string, std::string> pair = { key, arg };

    std::unique_lock<std::mutex> lock(mutex);
    keymap.erase(key);
    keymap.insert(pair);
}

std::string keymap::get(std::string key) {
    std::string res = "";
    auto i = keymap.find(tolower(key));
    if (i != keymap.end()) {
        res += i->second;
    }
    return res;
}

std::string keymap::dump() {
    std::string res = "";
    for (auto& item: keymap) {
        res += item.first;
        res += ": ";
        res += item.second;
        res += "\r\n";
    }
    return res;
}

} // namespace srv6
