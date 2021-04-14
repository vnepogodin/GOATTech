#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include <string>
#include <vector>
#include <vnepogodin/header.hpp>

namespace vnepogodin {
namespace http {

// A request received from a client.
//
struct request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // REQUEST_HPP_
