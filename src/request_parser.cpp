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

#include <vnepogodin/request.hpp>
#include <vnepogodin/request_parser.hpp>

namespace vnepogodin {

// Check if a byte is an HTTP character.
//
constexpr auto is_char(int c) noexcept -> bool {
    return (c >= 0) && (c <= 127);
}

// Check if a byte is an HTTP control character.
//
constexpr auto is_ctl(int c) noexcept -> bool {
    return (c >= 0 && c <= 31) || (c == 127);
}

// Check if a byte is defined as an HTTP tspecial character.
//
constexpr auto is_tspecial(int c) -> bool {
    switch (c) {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
            return true;
        default:
            return false;
    }
}

// Check if a byte is a digit.
//
constexpr auto is_digit(int c) noexcept -> bool {
    return (c >= '0') && (c <= '9');
}

namespace http {

auto request_parser::consume(request& req, char input) -> boost::tribool {
    switch (m_state) {
        case method_start:
            if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return false;
            } else {
                m_state = method;
                req.method.push_back(input);
                return boost::indeterminate;
            }
        case method:
            if (input == ' ') {
                m_state = uri;
                return boost::indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return false;
            } else {
                req.method.push_back(input);
                return boost::indeterminate;
            }
        case uri:
            if (input == ' ') {
                m_state = http_version_h;
                return boost::indeterminate;
            } else if (is_ctl(input)) {
                return false;
            } else {
                req.uri.push_back(input);
                return boost::indeterminate;
            }
        case http_version_h:
            if (input == 'H') {
                m_state = http_version_t_1;
                return boost::indeterminate;
            }
        case http_version_t_1:
            if (input == 'T') {
                m_state = http_version_t_2;
                return boost::indeterminate;
            }
        case http_version_t_2:
            if (input == 'T') {
                m_state = http_version_p;
                return boost::indeterminate;
            }
        case http_version_p:
            if (input == 'P') {
                m_state = http_version_slash;
                return boost::indeterminate;
            }
        case http_version_slash:
            if (input == '/') {
                req.http_version_major = 0;
                req.http_version_minor = 0;
                m_state = http_version_major_start;
                return boost::indeterminate;
            }
        case http_version_major_start:
            if (is_digit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                m_state = http_version_major;
                return boost::indeterminate;
            }
        case http_version_major:
            if (input == '.') {
                m_state = http_version_minor_start;
                return boost::indeterminate;
            } else if (is_digit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                return boost::indeterminate;
            }
        case http_version_minor_start:
            if (is_digit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                m_state = http_version_minor;
                return boost::indeterminate;
            }
        case http_version_minor:
            if (input == '\r') {
                m_state = expecting_newline_1;
                return boost::indeterminate;
            } else if (is_digit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                return boost::indeterminate;
            }
        case expecting_newline_1:
            if (input == '\n') {
                m_state = header_line_start;
                return boost::indeterminate;
            }
        case header_line_start:
            if (input == '\r') {
                m_state = expecting_newline_3;
                return boost::indeterminate;
            } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
                m_state = header_lws;
                return boost::indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return false;
            } else {
                req.headers.push_back(header());
                req.headers.back().name.push_back(input);
                m_state = header_name;
                return boost::indeterminate;
            }
        case header_lws:
            if (input == '\r') {
                m_state = expecting_newline_2;
                return boost::indeterminate;
            } else if (input == ' ' || input == '\t') {
                return boost::indeterminate;
            } else if (is_ctl(input)) {
                return false;
            } else {
                m_state = header_value;
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        case header_name:
            if (input == ':') {
                m_state = space_before_header_value;
                return boost::indeterminate;
            } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
                return false;
            } else {
                req.headers.back().name.push_back(input);
                return boost::indeterminate;
            }
        case space_before_header_value:
            if (input == ' ') {
                m_state = header_value;
                return boost::indeterminate;
            }
        case header_value:
            if (input == '\r') {
                m_state = expecting_newline_2;
                return boost::indeterminate;
            } else if (is_ctl(input)) {
                return false;
            } else {
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n') {
                m_state = header_line_start;
                return boost::indeterminate;
            }
        case expecting_newline_3:
            return (input == '\n');
        default:
            return false;
    }
}
}  // namespace http
}  // namespace vnepogodin
