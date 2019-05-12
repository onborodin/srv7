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

#ifndef SRV_CONFIG_HPP
#define SRV_CONFIG_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <regex>

namespace srv {

class config {
    private:
        std::map<std::string, std::string> str_map;
        std::map<std::string, int> num_map;
        std::vector<std::string> split(const std::string& source, const std::string& delimiter);

    public:
        bool read(std::string filename);
        int num_value(const std::string& key);
        std::string str_value(const std::string& key);
        void set_default(std::string key, int value);
        void set_default(std::string key, std::string value);
    };

} // namespce srv
#endif
