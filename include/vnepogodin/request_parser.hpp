#ifndef REQUEST_PARSER_HPP_
#define REQUEST_PARSER_HPP_

#include <boost/logic/tribool.hpp>
#include <tuple>

namespace vnepogodin {
namespace http {

struct request;

// Parse incoming requests.
class request_parser {
 public:
    // Constructor
    //
    request_parser() = default;

    // Reset parser state.
    void reset() {
        m_state = method_start;
    }

    // Parse some data. The tribool return value is true when a complete request
    // has been parsed, false if the data is invalid, indeterminate when more
    // data is required. The InputIterator return value indicates how much of the
    // input has been consumed.
    template<typename InputIterator>
    std::tuple<boost::tribool, InputIterator> parse(request& req,
                                                    InputIterator begin,
                                                    InputIterator end) {
        while (begin != end) {
            boost::tribool result = consume(req, *begin++);
            if (result || !result)
                return std::make_tuple(result, begin);
        }
        boost::tribool result = boost::indeterminate;
        return std::make_tuple(result, begin);
    }

 private:
    // The current state of the parser.
    //
    enum {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3
    } m_state = method_start;

    // Handle the next character of input.
    //
    auto consume(request& req, char input) -> boost::tribool;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // REQUEST_PARSER_HPP_
