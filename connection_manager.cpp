//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"

namespace server {

connection_manager::connection_manager() {
}

void connection_manager::start(connection_ptr c) {
    connections.insert(c);
    c->start();
}

void connection_manager::stop(connection_ptr c) {
    connections.erase(c);
    c->stop();
}

void connection_manager::stop_all() {
    for (auto c: connections) {
        c->stop();
    }
    connections.clear();
}

} // namespace server
