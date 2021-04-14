#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <vnepogodin/connection.hpp>
#include <vnepogodin/request_handler.hpp>
#include <vnepogodin/service_pool.hpp>

namespace vnepogodin {
namespace http {

class server {
 public:
    // Construct the server to listen on the specified TCP address and port, and
    // serve up files from the given directory.
    //
    explicit server(const std::string_view&, const std::string_view&, const uint16_t&);

    virtual ~server() = default;

    inline void run() {
        m_service_pool.run();
    }

    server(const server&) = delete;
    constexpr auto operator=(const server&) -> server& = delete;

 private:
    // Initiate an asynchronous accept operation.
    void start_accept();

    // Handle completion of an asynchronous accept operation.
    void handle_accept(const boost::system::error_code& e);

    // Handle a request to stop the server.
    void handle_stop();

    // Perform asynchronous operations.
    service_pool m_service_pool;

    // Register for process termination notifications.
    boost::asio::signal_set m_signals;

    // Listen for incoming connections.
    boost::asio::ip::tcp::acceptor m_acceptor;

    // The next connection to be accepted.
    connection_ptr m_conn_next{};

    // Incoming requests handler.
    request_handler m_req_handler{};
};

}  // namespace http
}  // namespace vnepogodin

#endif  // SERVER_HPP_
