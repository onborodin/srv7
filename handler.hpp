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

#ifndef SRV_HANDLER_HPP
#define SRV_HANDLER_HPP

#include <iostream>
#include <filesystem>

#include "cover.hpp"
#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include "config.hpp"

namespace srv {

class handler {
    private:
        http::request request;
        http::response response;
        std::shared_ptr<srv::cover> cover;

        std::string content_type(std::string& path);
    public:
        handler(std::shared_ptr<srv::cover> cover);

        void route(http::request& request, http::response& response);
        void prepare_file(http::request& request, http::response& response);
        void prepare_notfound(http::request& request, http::response& response);

        void log_response(http::request& request, http::response& response);
};

} // namespace srv

#endif
