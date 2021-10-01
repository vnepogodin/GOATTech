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

#include <QAudioInput>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QUrl>

namespace vnepogodin {
class Recorder final {
 public:
    Recorder() = default;
    explicit Recorder(const std::string_view& device_name) {
        QAudioInput* audioInput = new QAudioInput();
        m_media_session->setAudioInput(audioInput);
        m_media_session->setRecorder(m_media_recorder);
        m_media_recorder->setOutputLocation(QUrl::fromLocalFile("/tmp/capture"));
    }

    inline void record() noexcept { m_media_recorder->record(); }
    inline void stop() noexcept {
        if (m_media_recorder->recorderState() == QMediaRecorder::RecordingState) {
            m_media_recorder->stop();
        }
    }

    virtual ~Recorder() = default;

 private:
    QMediaRecorder* m_media_recorder{new QMediaRecorder};
    std::unique_ptr<QMediaCaptureSession> m_media_session{new QMediaCaptureSession};
};
}  // namespace vnepogodin

#endif  // RECORDER_HPP
