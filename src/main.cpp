//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vnepogodin/server.hpp>   // for server

#include <bits/exception.h>        // for exception
#include <boost/lexical_cast.hpp>  // for lexical_cast
#include <iostream>                // for size_t, operator<<, cerr, ostream
#include <string>                  // for allocator, char_traits
#include <string_view>

auto main(const int argc, char** argv) -> int32_t {
    // Initialize the server.
    constexpr uint16_t num_threads{1};
    http::server2::server s("0.0.0.0", "7000", ".", num_threads);

    // Run the server until stopped.
    s.run();
}
