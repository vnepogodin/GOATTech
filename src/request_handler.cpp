//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vnepogodin/html.hpp>
#include <vnepogodin/mime_types.hpp>
#include <vnepogodin/reply.hpp>
#include <vnepogodin/request.hpp>
#include <vnepogodin/request_handler.hpp>

#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <sstream>
#include <string>

namespace http {
namespace server2 {

request_handler::request_handler(const std::string& doc_root)
  : doc_root_(doc_root) {
}

std::string request_handler::format_time(std::time_t t) {
    using namespace boost::posix_time;
    static std::locale loc(std::cout.getloc(),
                           new time_facet("%Y/%m/%d %H:%M:%S"));
    std::basic_stringstream<char> ss;
    ss.imbue(loc);

    ss << boost::date_time::c_local_adjustor<ptime>::utc_to_local(from_time_t(t));

    return ss.str();
}

std::string request_handler::size_string(const uint64_t& bytes) {
    static const char* c[4]{"B", "K", "M", "G"};
    static const uint64_t u[4]{1, 1 << 10, 1 << 20, 1 << 30};
    uint32_t i = 0;
    for (uint64_t test = bytes; i < 4U; ++i) {
        test = test >> 10;
        if (test <= 1) {
            break;
        }
    }

    char buf[16];
    if (i > 0) {
        sprintf(buf, "%.2f %s", bytes * 1.00 / u[i], c[i]);
    } else {
        sprintf(buf, "%d B", bytes);
    }
    return std::string(buf);
}

bool request_handler::compare_nocase(const boost::filesystem::path& first, const boost::filesystem::path& second) {
    uint32_t i = 0;
    const auto& _first = first.native();
    const auto& _second = second.native();
    while ((i < _first.length()) && (i < _second.length())) {
        if (tolower(_first[i]) < tolower(_second[i]))
            return true;
        else if (tolower(_first[i]) > tolower(_second[i]))
            return false;
        ++i;
    }
    return (_first.length() < _second.length());
}

#ifdef WIN32
std::string request_handler::ansi_to_utf8(const std::string& ansi) {
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

std::string request_handler::utf8_to_ansi(const std::string& utf8) {
    int32_t len = utf8.length();
    if (len == 0)
        return std::string();

    wchar_t* unicode = new wchar_t[len + 1];
    len = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.length(), unicode, len);
    unicode[len] = 0;

    int32_t ansi_len = len * 4;
    char* ansi = new char[ansi_len + 1];
    ansi_len = ::WideCharToMultiByte(CP_ACP, 0, unicode, len, ansi, ansi_len, NULL, NULL);
    ansi[ansi_len] = 0;

    std::string ret(ansi, ansi_len);
    delete[] unicode;
    delete[] ansi;

    return ret;
}
#endif  // WIN32

void request_handler::handle_request(const request& req, reply& rep) {
    reply::status_type s = reply::ok;
    do {
        std::string request_path;
        if (!url_decode(req.uri, request_path)) {
            s = reply::bad_request;
            break;
        }

#ifdef WIN32
        request_path = utf8_to_ansi(request_path);
#endif

        // Request path must be absolute and not contain "..".
        if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos) {
            s = reply::bad_request;
            break;
        }

        std::string full_path = doc_root_ + request_path;
        boost::filesystem::path p(full_path);

        if (boost::filesystem::is_directory(p)) {
            if (boost::filesystem::exists(boost::filesystem::path(full_path + "index.html"))) {
                p += "index.html";
            } else if (boost::filesystem::exists(boost::filesystem::path(full_path + "index.htm"))) {
                p += "index.htm";
            } else {
                std::vector<file_info> fileinfos;
                {
                    file_info finfo;
                    finfo.filename = request_path;
                    finfo.is_folder = true;
                    finfo.url = url_encode(request_path);
                    finfo.size = "";
                    finfo.date = "";
                    fileinfos.push_back(finfo);
                }

                std::vector<boost::filesystem::path> files;
                std::copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), std::back_inserter(files));
                std::sort(files.begin(), files.end(), compare_nocase);

                for (std::vector<boost::filesystem::path>::const_iterator it(files.begin()); it != files.end(); ++it) {
                    try {
                        if (boost::filesystem::is_directory(*it)) {
                            std::time_t mtime = boost::filesystem::last_write_time(*it);

                            file_info finfo;
                            finfo.is_folder = true;
                            finfo.filename = it->filename().string();
                            finfo.url = url_encode(finfo.filename);
                            finfo.size = "";
                            finfo.date = format_time(mtime);
                            fileinfos.push_back(finfo);
                        }
                    }
                    catch (...) {
                    }
                }

                for (std::vector<boost::filesystem::path>::const_iterator it(files.begin()); it != files.end(); ++it) {
                    try {
                        if (boost::filesystem::is_regular_file(*it)) {
                            std::time_t mtime = boost::filesystem::last_write_time(*it);

                            file_info finfo;
                            finfo.is_folder = false;
                            finfo.filename = it->filename().string();
                            finfo.url = url_encode(finfo.filename);
                            finfo.size = size_string(boost::filesystem::file_size(*it));
                            finfo.date = format_time(mtime);
                            fileinfos.push_back(finfo);
                        }
                    }
                    catch (...) {
                    }
                }

#ifdef WIN32
                rep.content.append(ansi_to_utf8(file_contents_to_html(fileinfos)));
#else
                rep.content.append(file_contents_to_html(fileinfos));
#endif
                rep.status = reply::ok;
                rep.headers.resize(2);
                rep.headers[0].name = "Content-Length";
                rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
                rep.headers[1].name = "Content-Type";
                rep.headers[1].value = mime_types::extension_to_type("html");
                break;
            }
        }

        if (!boost::filesystem::exists(p)) {
            s = reply::not_found;
            break;
        }

        std::shared_ptr<boost::interprocess::file_mapping> fm;

        try {
            fm = std::shared_ptr<boost::interprocess::file_mapping>(new boost::interprocess::file_mapping(p.string().c_str(), boost::interprocess::read_only));
        }
        catch (...) {
            s = reply::not_found;
            break;
        }

        std::string extension = p.extension().string();
        if (extension[0] == '.') {
            extension = extension.substr(1);
        }

        uint64_t file_len = boost::filesystem::file_size(p);
        uint64_t processed = 0;
        if (file_len > 0) {
            uint64_t mlen = (file_len > MEM_CACHE_SIZE) ? MEM_CACHE_SIZE : file_len;
            boost::interprocess::mapped_region region(*fm, boost::interprocess::read_only, 0, mlen);
            processed = region.get_size();
            rep.content.assign((const char*)region.get_address(), processed);
        }

        rep.status = reply::ok;
        rep.headers.resize(2);
        rep.headers[0].name = "Content-Length";
        rep.headers[0].value = boost::lexical_cast<std::string>(file_len);
        rep.headers[1].name = "Content-Type";
        rep.headers[1].value = mime_types::extension_to_type(extension);

        rep.file.file_mapping = fm;
        rep.file.file_size = file_len;
        rep.file.processed = processed;

    } while (0);
    if (reply::ok != s) {
        rep = reply::stock_reply(s);
        return;
    }
}

bool request_handler::url_decode(const std::string& in, std::string& out) {
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%') {
            if (i + 3 <= in.size()) {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}

std::string request_handler::url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (c == '-' ||
            c == '.' ||
            (c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            c == '_' ||
            (c >= 'a' && c <= 'z') ||
            c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << '%' << std::setw(2) << int((unsigned char)c);
    }

    return escaped.str();
}

}  // namespace server2
}  // namespace http
