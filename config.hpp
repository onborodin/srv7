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


#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>
#include "keymap.hpp"

namespace srv {

class config {
    public:
        std::string port;
        std::string address;
        std::string keyfile;
        std::string crtfile;
        std::string pubdir;
        std::string index;
        int poolsize;
        int backlog;
};

class ptrbox {
    public:
        std::shared_ptr<::srv::keymap> filemap;
        std::shared_ptr<::srv::config> config;
};

} // namespace srv

#endif
