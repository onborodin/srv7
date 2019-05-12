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

#ifndef SRV_COVER_HPP
#define SRV_COVER_HPP

#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

#include "autoconf.hpp"
#include "config.hpp"
#include "keymap.hpp"
#include "logger.hpp"

namespace srv {

class cover : public std::enable_shared_from_this<srv::cover> {
    private:
        void fork_process();
        void set_userid(const std::string& owner);
        void change_pwd();
        void write_pid();
        void stdio_close();

        void init_config();
        void init_filemap();
        void init_logger();

    public:
        std::shared_ptr<srv::config> config;
        std::shared_ptr<srv::logger> logger;
        std::shared_ptr<srv::keymap> filemap;

        void start();
        void stop();
};

} // namespace srv

#endif
