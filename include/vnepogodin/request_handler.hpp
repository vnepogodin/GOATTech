/*
 * Copyright (C) 2021 Vladislav Nepogodin
 *
 * This file is part of SportTech overlay project.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include <vnepogodin/reply.hpp>

#include <string_view>

namespace vnepogodin {
namespace http {

struct request_handler {
    // Construct with a directory containing files to be served.
    explicit request_handler() = default;

    // Handle a request and produce a reply.
    void handle_request(reply& rep);

    request_handler(const request_handler&) = delete;
    constexpr auto operator=(const request_handler&) -> request_handler& = delete;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // REQUEST_HANDLER_HPP_
