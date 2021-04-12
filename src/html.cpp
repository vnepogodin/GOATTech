#include <vnepogodin/html.hpp>

#include <memory>   // for allocator_traits<>::value_type
#include <sstream>  // for operator<<, basic_ostream, stringstream

constexpr const char* html_str =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset=\"utf-8\">"
    "<style>"
    "body{"
    "background: aqua;"
    "font-family:Hack,monospace;"
    "}"
    "</style>"
    "<title>Hello World</title>"
    "</head>"
    "<body>"
    "</body>"
    "</html>";

const char* get_html_str() {
    return html_str;
}

std::string file_contents_to_html(const std::vector<file_info>& files) {
    std::stringstream ss;
    std::string::size_type len = files[0].url.length();

    ss << get_html_str() << '\n';

    return ss.str();
}
