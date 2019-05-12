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
#include <filesystem>
#include <string>
#include <memory>

#include <stdint.h>
#include <unistd.h>
#include <pwd.h>

#include "cover.hpp"

#include "autoconf.hpp"
#include "config.hpp"
#include "keymap.hpp"
#include "logger.hpp"

#include "server.hpp"

namespace srv {

void cover::fork_process() {
    ::pid_t pid = ::fork();
    if (pid < 0) {
        throw std::runtime_error("cannot fork process");
    }
    if (pid > 0) {
        exit(0);
    }
    ::setsid();
}

void cover::set_userid(const std::string& owner) {
    ::passwd *pw = nullptr;
    uid_t uid = -1;
    while ((pw = ::getpwent()) != NULL) {
        if (owner == std::string(pw->pw_name)) {
            uid = pw->pw_uid;
            break;
        }
    }
    ::endpwent();
    if (uid == -1) {
        std::string msg;
        msg = "cannot found process owner " + owner;
        throw std::runtime_error(msg);
    }
    if (::setuid(uid) < 0) {
        throw std::runtime_error("cannot set new process owner");
    }
}


void cover::change_pwd() {
    std::filesystem::current_path("/");
}

void cover::write_pid() {
    auto pidfile = config->str_value("pidfile");
    if (pidfile.length() == 0) {
        throw std::runtime_error("zero length of pid path");
    }

    std::ofstream fpid(pidfile, std::ofstream::out|std::ofstream::trunc);
    if (!(fpid && fpid.is_open())) {
        std::string msg;
        msg = "cannot write pid file " + pidfile;
        throw std::runtime_error(msg);
    }
    fpid << ::getpid();
    fpid.flush();
    fpid.close();
}

void cover::stdio_close() {
    //std::freopen("/dev/null", "r", stdin);
    //std::freopen("/dev/null", "w", stdout);
    //std::freopen("/dev/null", "w", stderr);
}

void cover::init_config() {
    config = std::make_shared<srv::config>();

    config->set_default("address", "0.0.0.0");
    config->set_default("port", "1024");
    config->set_default("threads", 12);
    config->set_default("backlog", 2048);
    config->set_default("index", "index.html");

    config->set_default("logfile", APP_LOGDIR "srv.log");
    config->set_default("pidfile", APP_LOGDIR "srv.pid");
    config->set_default("crtfile", APP_CONFDIR "server.crt");
    config->set_default("keyfile", APP_CONFDIR "server.key");
    config->set_default("pubdir", APP_DATADIR "public");

    config->read(APP_CONFDIR "srv.conf");
}

void cover::init_filemap() {
    filemap = std::make_shared<srv::keymap>();
    filemap->set("html", "text/html");
    filemap->set("js", "application/javascript");
    filemap->set("json", "application/json");
    filemap->set("jpg", "image/jpeg");
    filemap->set("png", "image/png");
    filemap->set("css", "text/css");
    filemap->set("cpp", "text/plain");
    filemap->set("ico", "image/x-icon");
    filemap->set("svg", "image/svg+xml");
}

void cover::init_logger() {
    auto logfile = config->str_value("logfile");
    if (logfile.length() == 0) {
        throw std::runtime_error("zero length of log file path");
    }
    logger = std::make_shared<srv::logger>(logfile);
}


void cover::start() {
    //fork_process();
    init_config();
    init_filemap();
    init_logger();
    stdio_close();
    write_pid();
    logger->log("application start");
    srv::server server(shared_from_this());
    server.run();
}

void cover::stop() {
    logger->log("application stop");
    logger->stop();
}

} // namespace srv
