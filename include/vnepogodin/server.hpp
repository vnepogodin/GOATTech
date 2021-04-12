//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_SERVER_HPP
#define HTTP_SERVER2_SERVER_HPP

#include <vnepogodin/connection.hpp>
#include <vnepogodin/io_service_pool.hpp>
#include <vnepogodin/request_handler.hpp>
#include <vector>

namespace http {
namespace server2 {

/// The top-level class of the HTTP server.
class server {
 public:
    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit server(const std::string_view&, const std::string_view&, const std::string_view&, const uint16_t&);

    /// Run the server's io_service loop.
    void run();

    server(const server&) = delete;
    constexpr auto operator=(const server&) -> server& = delete;

 private:
    /// Initiate an asynchronous accept operation.
    void start_accept();

    /// Handle completion of an asynchronous accept operation.
    void handle_accept(const boost::system::error_code& e);

    /// Handle a request to stop the server.
    void handle_stop();

    /// The pool of io_service objects used to perform asynchronous operations.
    io_service_pool m_service_pool;

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set m_signals;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor m_acceptor;

    /// The next connection to be accepted.
    connection_ptr m_conn_next;

    /// The handler for all incoming requests.
    request_handler m_req_handler;
};

}  // namespace server2
}  // namespace http

#endif  // HTTP_SERVER2_SERVER_HPP
