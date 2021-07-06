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

#ifndef UIOHOOK_HELPER_HPP
#define UIOHOOK_HELPER_HPP

#include <vnepogodin/buffer.hpp>

#include <atomic>
#include <mutex>

#include <uiohook.h>

namespace uiohook {

extern std::atomic<bool> hook_state;
extern std::mutex buffer_mutex;
extern vnepogodin::buffer buf;

bool logger_proc(unsigned level, const char* format, ...);

void dispatch_proc(uiohook_event* event);
bool start();
void stop();

}  // namespace uiohook

#endif  // UIOHOOK_HELPER_HPP
