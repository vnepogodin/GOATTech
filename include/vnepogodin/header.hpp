#ifndef HEADER_HPP_
#define HEADER_HPP_

#include <string>

namespace vnepogodin {
namespace http {

struct header {
    std::string name;
    std::string value;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // HEADER_HPP_
