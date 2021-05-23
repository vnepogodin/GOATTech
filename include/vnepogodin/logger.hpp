// Copyright (C) 2021 Vladislav Nepogodin
//
// This file is part of SportTech overlay project.
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

#ifdef _WIN32
#include <Windows.h>
#include <tlhelp32.h> // PROCESSENTRY32W, CreateToolhelp32Snapshot, Process32FirstW, Process32NextW
#else
#include <cstring>
#include <dirent.h>
#include <vector>
#endif
#include <chrono>
#include <fstream>
#include <vnepogodin/thirdparty/json.hpp>

namespace {
std::string_view get_process_list() {
#ifdef _WIN32
    // Take a snapshot of all processes in the system.
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // Set the size of the structure before using it.
    PROCESSENTRY32W pe32{};
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32FirstW(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);  // clean the snapshot object
        return "Unknown process";
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    const std::unordered_map<std::string_view, std::string_view> process_map = {
        {"dota.exe", "Dota"},
        {"csgo.exe", "Counter-Strike: Global Offensive"},
        {"lol.exe", "League of Legends"},
        {"fortnite.exe", "Fortnite"}};
    do {
        std::wstring wide = pe32.szExeFile;
        std::string exe(wide.begin(), wide.end());
        for (const auto& [process, name] : process_map) {
            if (process == exe) {
                CloseHandle(hProcessSnap);
                return name;
            }
        }
    } while (Process32NextW(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

#else
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
#endif
    return "Unknown process";
}
}  // namespace

namespace vnepogodin {
class Logger {
 public:
    Logger() {
        /* clang-format off */
        this->j = {
            {"name", get_process_list()},
            {"timestamp", 0},
            {"keys", nlohmann::json::array()}};
        /* clang-format on */

#ifdef _WIN32
        char buf[100]{};
        GetTempPathA(85, buf);
        const std::string file = std::string(buf) + "db.json";
#else
        static constexpr std::string_view path = "/tmp/db.json";
#endif

        this->log_output.open(file.data(), std::ofstream::app);
    }

    ~Logger() = default;

    inline auto write() -> void {
      if (!this->j["keys"].empty()) {
        this->j.at("name") = get_process_list();
      this->j.at("timestamp") = std::chrono::system_clock::to_time_t(
          std::chrono::system_clock::now());
      this->log_output << this->j << '\n';
      this->j["keys"].clear();
    }
    }

    inline auto add_key(const std::string_view value) -> void {
        this->j["keys"].push_back(value);
    }

    inline auto close() -> void {
        this->log_output.close();
    }

 private:
    std::ofstream log_output{};
    nlohmann::json j;
};
}  // namespace vnepogodin

#endif  // LOGGER_HPP
