//
// io_service_pool.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER2_IO_SERVICE_POOL_HPP
#define HTTP_SERVER2_IO_SERVICE_POOL_HPP

#include <boost/asio.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>

namespace http {
namespace server2 {

/// A pool of io_service objects.
class io_service_pool {
 public:
    /// Construct the io_service pool.
    explicit io_service_pool(std::size_t pool_size);

    /// Run all io_service objects in the pool.
    void run();

    /// Stop all io_service objects in the pool.
    void stop();

    /// Get an io_service to use.
    boost::asio::io_service& get_io_service();
    io_service_pool(const io_service_pool&) = delete;
    constexpr auto operator=(const io_service_pool&) -> io_service_pool& = delete;
 private:
    using io_service_ptr = std::shared_ptr<boost::asio::io_service>;
    using work_ptr = std::shared_ptr<boost::asio::io_service::work>;

    /// The pool of io_services.
    std::vector<io_service_ptr> io_services_;

    /// The work that keeps the io_services running.
    std::vector<work_ptr> work_;

    /// The next io_service to use for a connection.
    std::size_t next_io_service_;
};

}  // namespace server2
}  // namespace http

#endif  // HTTP_SERVER2_IO_SERVICE_POOL_HPP
