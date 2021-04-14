#include <vnepogodin/html.hpp>
#include <vnepogodin/request_handler.hpp>

#include <boost/lexical_cast.hpp>

#ifdef WIN32
# include <string>
#endif  // WIN32

namespace vnepogodin {
namespace http {

#ifdef WIN32
static auto ansi_to_utf8(const std::string_view& ansi) noexcept -> std::string {
    int32_t len = ansi.length();
    if (len == 0)
        return std::string();

    wchar_t* unicode = new wchar_t[len + 1];
    len = ::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), ansi.length(), unicode, len);
    unicode[len] = 0;

    int32_t utf8_len = len * 4;
    char* utf8 = new char[utf8_len + 1];
    utf8_len = ::WideCharToMultiByte(CP_UTF8, 0, unicode, len, utf8, utf8_len, NULL, NULL);
    utf8[utf8_len] = 0;

    std::string ret(utf8, utf8_len);
    delete[] unicode;
    delete[] utf8;

    return ret;
}
#endif  // WIN32

void request_handler::handle_request(reply& rep) {
#ifdef WIN32
    rep.content.append(ansi_to_utf8(get_html()));
#else
    rep.content.append(get_html());
#endif
    rep.status = reply::ok;
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
    rep.headers[1].name = "Content-Type";
    rep.headers[1].value = "text/html";
}
}  // namespace http
}  // namespace vnepogodin
