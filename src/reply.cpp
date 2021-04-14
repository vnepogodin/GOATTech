#include <vnepogodin/reply.hpp>

#include <boost/lexical_cast.hpp>

namespace vnepogodin {
namespace http {
namespace status_strings {

constexpr char ok[] =
    "HTTP/1.0 200 OK\r\n";
constexpr char created[] =
    "HTTP/1.0 201 Created\r\n";
constexpr char accepted[] =
    "HTTP/1.0 202 Accepted\r\n";
constexpr char no_content[] =
    "HTTP/1.0 204 No Content\r\n";
constexpr char multiple_choices[] =
    "HTTP/1.0 300 Multiple Choices\r\n";
constexpr char moved_permanently[] =
    "HTTP/1.0 301 Moved Permanently\r\n";
constexpr char moved_temporarily[] =
    "HTTP/1.0 302 Moved Temporarily\r\n";
constexpr char not_modified[] =
    "HTTP/1.0 304 Not Modified\r\n";
constexpr char bad_request[] =
    "HTTP/1.0 400 Bad Request\r\n";
constexpr char unauthorized[] =
    "HTTP/1.0 401 Unauthorized\r\n";
constexpr char forbidden[] =
    "HTTP/1.0 403 Forbidden\r\n";
constexpr char not_found[] =
    "HTTP/1.0 404 Not Found\r\n";
constexpr char internal_server_error[] =
    "HTTP/1.0 500 Internal Server Error\r\n";
constexpr char not_implemented[] =
    "HTTP/1.0 501 Not Implemented\r\n";
constexpr char bad_gateway[] =
    "HTTP/1.0 502 Bad Gateway\r\n";
constexpr char service_unavailable[] =
    "HTTP/1.0 503 Service Unavailable\r\n";

boost::asio::const_buffer to_buffer(reply::status_type status) {
    switch (status) {
        case reply::ok:
            return boost::asio::buffer(ok);
        case reply::created:
            return boost::asio::buffer(created);
        case reply::accepted:
            return boost::asio::buffer(accepted);
        case reply::no_content:
            return boost::asio::buffer(no_content);
        case reply::multiple_choices:
            return boost::asio::buffer(multiple_choices);
        case reply::moved_permanently:
            return boost::asio::buffer(moved_permanently);
        case reply::moved_temporarily:
            return boost::asio::buffer(moved_temporarily);
        case reply::not_modified:
            return boost::asio::buffer(not_modified);
        case reply::bad_request:
            return boost::asio::buffer(bad_request);
        case reply::unauthorized:
            return boost::asio::buffer(unauthorized);
        case reply::forbidden:
            return boost::asio::buffer(forbidden);
        case reply::not_found:
            return boost::asio::buffer(not_found);
        case reply::internal_server_error:
            return boost::asio::buffer(internal_server_error);
        case reply::not_implemented:
            return boost::asio::buffer(not_implemented);
        case reply::bad_gateway:
            return boost::asio::buffer(bad_gateway);
        case reply::service_unavailable:
            return boost::asio::buffer(service_unavailable);
        default:
            return boost::asio::buffer(internal_server_error);
    }
}

}  // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

}  // namespace misc_strings

std::vector<boost::asio::const_buffer> reply::to_buffers() {
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(status_strings::to_buffer(status));
    for (const auto& head : headers) {
        buffers.push_back(boost::asio::buffer(head.name));
        buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
        buffers.push_back(boost::asio::buffer(head.value));
        buffers.push_back(boost::asio::buffer(misc_strings::crlf));
    }
    buffers.push_back(boost::asio::buffer(misc_strings::crlf));
    buffers.push_back(boost::asio::buffer(content));
    return buffers;
}

namespace stock_replies {

constexpr char ok[] = "";
constexpr char internal_server_error[] =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";

std::string to_string(reply::status_type status) {
    switch (status) {
        case reply::ok:
            return ok;
        case reply::internal_server_error:
            return internal_server_error;
        default:
            return internal_server_error;
    }
}

}  // namespace stock_replies

reply reply::stock_reply(reply::status_type status) {
    reply rep;
    rep.status = status;
    rep.content = stock_replies::to_string(status);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = "text/html";
    return rep;
}

}  // namespace http
}  // namespace vnepogodin
