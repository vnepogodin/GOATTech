#include <vnepogodin/server.hpp>  // for server

using namespace vnepogodin;

auto main() -> int32_t {
    // Initialize the server.
    constexpr uint16_t num_threads{2};
    http::server s("0.0.0.0", "7000", num_threads);

    // Run the server until stopped.
    s.run();
}
