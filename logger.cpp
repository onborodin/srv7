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
#include <fstream>
#include <sstream>
#include <functional>
#include <condition_variable>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <iomanip>

#include "logger.hpp"

namespace srv {

std::string logger::timestamp() {
    std::time_t now = std::time(0);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&now), "%Y-%m-%d %T %Z");
    return ss.str();
}

logger::logger(std::string path, int num_threads) : path(path) {
    using mode = std::fstream;
    file = std::make_shared<std::ofstream>(
        path, mode::out | mode::app
    );
    if (!file->is_open()) {
        throw std::runtime_error("cannot open log file");
    }
    auto worker = [&]{
        while(true) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [&]{ return !queue.empty(); });
            while(!queue.empty()) {
                auto msg = queue.front();
                queue.pop();
                cv.notify_one();
                std::cerr << msg.content << std::endl;
                *file << msg.content << std::endl;
            }
        }
    };

    for (std::size_t i = 0; i < num_threads; ++i) {
        auto t = new std::thread(std::move(worker));
        std::shared_ptr<std::thread> thread(std::move(t));
        threads.push_back(thread);
    }
    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->detach();
    }
}

void logger::log(std::string msg) {
    message m(timestamp() + " " + msg);
    auto func = [&]{
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(m);
    };
    func();
    cv.notify_all();
}

} //namespace srv
