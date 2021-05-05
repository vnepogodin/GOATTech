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
#include <tlhelp32.h>
#endif
#include <fstream>
#include <vnepogodin/thirdparty/json.hpp>

#ifdef _WIN32
namespace {
std::string_view get_process_list() {
    // Take a snapshot of all processes in the system.
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // Set the size of the structure before using it.
    PROCESSENTRY32 pe32{};
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
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
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return "Unknown process";
}
}  // namespace
#endif
class Logger {
 public:
    Logger() {
        this->j = {
            {"name", ""},
            {"numOfTouch", {
                {"w_button", 0},
                {"a_button", 0},
                {"s_button", 0},
                {"d_button", 0},
                {"q_button", 0},
                {"e_button", 0},
                {"shift_button", 0},
                {"ctrl_button", 0},
                {"space_button", 0},
                {"left_button", 0},
                {"right_button", 0},
                {"middle_button", 0},
                {"x1_button", 0},
                {"x2_button", 0},
        }}};
#ifdef _WIN32
        GetTempPathA(85, path);
        j.at("name") = get_process_list();
#endif
    }

    ~Logger() {
        this->write();
    }

    void write() {
        const std::string file = std::string(path) + "db.json";
        std::ofstream out(file, std::ios_base::app);
        out << this->j << '\n';
    }

    auto get(const char* path) -> nlohmann::json& {
        return j[path];
    }

 private:
    char path[100]{};
    nlohmann::json j{};
};

#endif  // LOGGER_HPP
