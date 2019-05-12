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

#ifndef SRV_LOGGER_HPP
#define SRV_LOGGER_HPP

#include <fstream>
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

namespace srv {

class logger {
    private:
        class message {
            public:
                std::string content;
                message(std::string msg) : content(msg) {}
        };
        std::queue<message> queue;
        std::condition_variable cv;
        std::mutex mutex;
        std::vector<std::shared_ptr<std::thread>> threads;
        std::string path;
        std::shared_ptr<std::ofstream> file;
        bool shutdown = false;

        std::string timestamp();
    public:
        logger(std::string path, int num_threads = 2);
        void log(std::string msg);
        void stop();
};

} // namespace srv
#endif
