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

#include "manager.hpp"

namespace srv6 {

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

} // namespace srv6
