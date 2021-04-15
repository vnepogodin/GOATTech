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
