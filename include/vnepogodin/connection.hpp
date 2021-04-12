//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_CONNECTION_HPP
#define HTTP_SERVER2_CONNECTION_HPP

#include <vnepogodin/reply.hpp>
#include <vnepogodin/request.hpp>
#include <vnepogodin/request_handler.hpp>
#include <vnepogodin/request_parser.hpp>

#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace http {
namespace server2 {

/// Represents a single connection from a client.
class connection
  : public boost::enable_shared_from_this<connection> {
 public:
    /// Construct a connection with the given io_service.
    explicit connection(boost::asio::io_service& io_service,
                        request_handler& handler);

    /// Get the socket associated with the connection.
    inline auto socket() noexcept -> boost::asio::ip::tcp::socket&
    { return socket_; }

    /// Start the first asynchronous operation for the connection.
    void start();

    connection(const connection&) = delete;
    constexpr auto operator=(const connection&) -> connection& = delete;

 private:
    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code& e);

    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    /// The handler used to process the incoming request.
    request_handler& request_handler_;

    /// Buffer for incoming data.
    boost::array<char, 8192> buffer_;

    /// The incoming request.
    request request_;

    /// The parser for the incoming request.
    request_parser request_parser_;

    /// The reply to be sent back to the client.
    reply reply_;
};

using connection_ptr = boost::shared_ptr<connection>;

}  // namespace server2
}  // namespace http

#endif  // HTTP_SERVER2_CONNECTION_HPP
