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

#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <memory>
#include <string_view>

#include <QAudioRecorder>
#include <QUrl>

namespace vnepogodin {
class Recorder final {
 public:
    Recorder() = default;
    explicit Recorder(const std::string_view& device_name)
      : m_audioRecorder(new QAudioRecorder) {
        m_audioRecorder->setAudioInput(device_name.data());
        m_audioRecorder->setOutputLocation(QUrl::fromLocalFile("/tmp/capture"));
    }

    inline void record() noexcept { m_audioRecorder->record(); }
    inline void stop() noexcept { m_audioRecorder->stop(); }

    inline void toggle() noexcept {
        if (m_audioRecorder->state() == QMediaRecorder::StoppedState) {
            this->record();
            return;
        }
        this->stop();
    }

    virtual ~Recorder() = default;

 private:
    std::unique_ptr<QAudioRecorder> m_audioRecorder;
};
}  // namespace vnepogodin

#endif  // RECORDER_HPP
