// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of GOATTech project.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstring>
#include <dirent.h>

#include <chrono>
#include <fstream>
#include <vector>

#include <vnepogodin/thirdparty/json.hpp>

namespace {
std::string_view get_process_list() {
    DIR* dir_proc = opendir("/proc/");
    if (dir_proc == NULL) {
        return "Unknown process";
    }

    // Retrieve information about the first process,
    // and exit if unsuccessful
    struct dirent* dir_entity = readdir(dir_proc);

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    const std::unordered_map<std::string_view, std::string_view> process_map = {
        {"dota", "Dota"},
        {"csgo", "Counter-Strike: Global Offensive"}};
    do {
        if (dir_entity->d_type == DT_DIR) {
            const std::string& chrarry_CommandLinePath = "/proc/" + std::string(dir_entity->d_name) + "/cmdline";
            std::ifstream in(std::string(chrarry_CommandLinePath), std::ios_base::in);
            if (in) {
                const std::vector<std::uint8_t> buffer(std::istreambuf_iterator<char>(in), {});
                std::string exe(reinterpret_cast<const char*>(buffer.data()), buffer.size());
                const char* found = strrchr(exe.data(), '/');
                if (found) {
                    exe = found + 1;
                }

                for (const auto& [process, name] : process_map) {
                    if (process == exe) {
                        closedir(dir_proc);
                        return name;
                    }
                }
            }
        }
    } while ((dir_entity = readdir(dir_proc)));

    closedir(dir_proc);
    return "Unknown process";
}
}  // namespace

namespace vnepogodin {
class Logger final {
 public:
    Logger() {
        /* clang-format off */
        m_json = {
            {"name", get_process_list()},
            {"timestamp", 0},
            {"keys", nlohmann::json::array()}};
        /* clang-format on */

        static constexpr std::string_view file = "/tmp/db.json";
        m_log_output.open(file.data(), std::ofstream::app);
    }

    virtual ~Logger() = default;

    inline auto write() -> void {
        if (!m_json["keys"].empty()) {
            m_json.at("name")      = get_process_list();
            m_json.at("timestamp") = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());
            m_log_output << m_json << '\n';
            m_json["keys"].clear();
        }
    }

    inline auto add_key(const std::string_view value) -> void {
        m_json["keys"].push_back(value);
    }

    inline auto close() -> void {
        m_log_output.close();
    }

 private:
    std::ofstream m_log_output{};
    nlohmann::json m_json;
};
}  // namespace vnepogodin

#endif  // LOGGER_HPP
