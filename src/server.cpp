//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vnepogodin/server.hpp>
#include <boost/bind.hpp>

namespace http {
namespace server2 {

server::server(const std::string_view& address, const std::string_view& port, const std::string_view& doc_root, const uint16_t& io_service_pool_size)
  : m_service_pool(io_service_pool_size),
    m_signals(m_service_pool.get_io_service()),
    m_acceptor(m_service_pool.get_io_service()),
    m_conn_next(),
    m_req_handler(doc_root.data())
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
#if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
#endif  // defined(SIGQUIT)
    m_signals.async_wait(boost::bind(&server::handle_stop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(m_acceptor.get_executor());
    boost::asio::ip::tcp::resolver::query query(address.data(), port.data());
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();

    start_accept();
}

void server::run() {
    m_service_pool.run();
}

void server::start_accept() {
    m_conn_next.reset(new connection(
        m_service_pool.get_io_service(),
        m_req_handler));
    m_acceptor.async_accept(m_conn_next->socket(),
                           boost::bind(&server::handle_accept, this, boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e) {
    if (!e) {
        m_conn_next->start();
    }

    start_accept();
}

void server::handle_stop() {
    m_service_pool.stop();
}

}  // namespace server2
}  // namespace http
