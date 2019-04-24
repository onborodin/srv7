//
// manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "manager.hpp"

namespace server {

manager::manager() {
}

void manager::start(connection_ptr c) {
    connections.insert(c);
    c->start();
}

void manager::stop(connection_ptr c) {
    connections.erase(c);
    c->stop();
}

void manager::stop_all() {
    for (auto c: connections) {
        c->stop();
    }
    connections.clear();
}

} // namespace server
