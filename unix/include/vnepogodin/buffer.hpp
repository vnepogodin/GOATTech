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

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include <cstring>
#include <vector>

namespace vnepogodin {
class buffer {
 public:
    using value_type      = std::uint8_t;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    explicit buffer(std::size_t len = 0) {
        if (len > 0)
            resize(len);
    }

    virtual ~buffer() = default;

    constexpr void reset() noexcept {
        m_read_pos  = 0;
        m_write_pos = 0;
    }

    constexpr void resize(std::size_t new_size) {
        m_buf.resize(new_size);
    }

    constexpr void write(const void* value, std::size_t size) {
        if (m_write_pos + size >= m_buf.size())
            resize(m_write_pos + size * 1.5);
        memcpy(m_buf.data() + m_write_pos, value, size);
        m_write_pos += size;
    }

    constexpr void read(void** dest, std::size_t size) {
        if (size + m_read_pos < m_buf.size()) {
            *dest = reinterpret_cast<void*>(data() + m_read_pos);
        }
    }

    template <class T>
    constexpr auto write(const T& val) -> T* {
        if (m_write_pos + sizeof(T) >= m_buf.size())
            resize(m_write_pos + sizeof(T) * 1.5);
        memcpy(data() + m_write_pos, &val, sizeof(T));
        auto* res = reinterpret_cast<T*>(data() + m_write_pos);
        m_write_pos += sizeof(T);
        return res;
    }

    template <class T>
    constexpr auto read() -> T* {
        if (sizeof(T) + m_read_pos < size()) {
            auto result = static_cast<T*>(data() + m_read_pos);
            m_read_pos += sizeof(T);
            return result;
        }
        return nullptr;
    }

    inline auto
    operator[](const std::size_t& idx) noexcept -> reference { return m_buf[idx]; }
    inline auto
    operator[](const std::size_t& idx) const noexcept -> const_reference { return m_buf[idx]; }

    constexpr std::size_t size() const noexcept { return m_buf.size(); }
    constexpr std::size_t write_pos() const noexcept { return m_write_pos; }
    constexpr std::size_t read_pos() const noexcept { return m_read_pos; }
    constexpr pointer data() { return m_buf.data(); }

 private:
    std::size_t m_write_pos = 0;
    std::size_t m_read_pos  = 0;

    std::vector<std::uint8_t> m_buf{};
};
}  // namespace vnepogodin

#endif  // BUFFER_HPP