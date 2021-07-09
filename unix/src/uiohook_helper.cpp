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

#include <vnepogodin/logger.hpp>
#include <vnepogodin/uiohook_helper.hpp>
#include <vnepogodin/utils.hpp>

#include <cstdarg>
#include <cstdio>

#include <uiohook.h>

namespace uiohook {
std::atomic<bool> hook_state;
std::mutex buffer_mutex;
vnepogodin::buffer buf;

static vnepogodin::Logger logger;

using namespace vnepogodin;
static inline std::uint32_t handle_key(const std::uint32_t& key_stroke) {
    for (const auto& code : utils::code_list) {
        if (code.first == key_stroke) {
            logger.add_key(code.second);
            return code.first;
        }
    }

    return utils::key_code::UNDEFINED;
}

bool logger_proc(unsigned int level, const char* format, ...) {
    bool status = false;

    va_list args;
    switch (level) {
#ifndef NDEBUG
    case LOG_LEVEL_INFO:
        va_start(args, format);
        status = vfprintf(stdout, format, args) >= 0;
        va_end(args);
        break;
#endif

    case LOG_LEVEL_WARN:
    case LOG_LEVEL_ERROR:
        va_start(args, format);
        status = vfprintf(stderr, format, args) >= 0;
        va_end(args);
        break;
    }

    return status;
}

void dispatch_proc(uiohook_event* const event) {
    std::lock_guard<std::mutex> lock(buffer_mutex);

    switch (event->type) {
    case EVENT_HOOK_ENABLED:
        // Lock the running mutex, so we know if the hook is enabled.
        hook_state = true;
        break;
    case EVENT_MOUSE_PRESSED:
        buf.write<uiohook_event>(*event);
        handle_key(event->data.mouse.button);
        break;
    case EVENT_KEY_PRESSED:
        buf.write<uiohook_event>(*event);
        handle_key(event->data.keyboard.keycode);
        break;
    case EVENT_MOUSE_RELEASED:
    case EVENT_MOUSE_CLICKED:
    case EVENT_MOUSE_MOVED:
    case EVENT_MOUSE_DRAGGED:
        buf.write<uiohook_event>(*event);
        break;
    case EVENT_KEY_TYPED:
    case EVENT_KEY_RELEASED:
        buf.write<uiohook_event>(*event);
        break;
    default:
        break;
    }
}

bool start() {
    hook_set_logger_proc(&logger_proc);
    hook_set_dispatch_proc(&dispatch_proc);

    const auto& status = hook_run();

    switch (status) {
    case UIOHOOK_SUCCESS:
        // We no longer block, so we need to explicitly wait for the thread to die.
        hook_state = true;
        return true;

    // System level errors.
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        return false;

    // X11 specific errors.
    case UIOHOOK_ERROR_X_OPEN_DISPLAY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to open X11 display. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to locate XRecord extension. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord range. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Unable to allocate XRecord context. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable XRecord context. (%#X)", status);
        return false;

    // Darwin specific errors.
    case UIOHOOK_ERROR_AXAPI_DISABLED:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to enable access for assistive devices. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_EVENT_PORT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple event port. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop source. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_GET_RUNLOOP:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to acquire apple run loop. (%#X)", status);
        return false;
    case UIOHOOK_ERROR_CREATE_OBSERVER:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to create apple run loop observer. (%#X)", status);
        return false;
    case UIOHOOK_FAILURE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        return false;
    default:
        return true; /* Unknown error but we can still try */
    }
}

void stop() {
    if (!hook_state)
        return;
    hook_state         = false;
    const auto& status = hook_stop();
    logger.write();
    logger.close();

    switch (status) {
    case UIOHOOK_ERROR_OUT_OF_MEMORY:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to allocate memory. (%#X)", status);
        break;
    case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] Failed to get XRecord context. (%#X)", status);
        break;
    case UIOHOOK_FAILURE:
        logger_proc(LOG_LEVEL_ERROR, "[uiohook] An unknown hook error occurred. (%#X)", status);
        break;
    default:
        break;
    }
}
}  // namespace uiohook
