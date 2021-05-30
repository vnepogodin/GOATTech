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

#include <vnepogodin/webui.hpp>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <QFile>
#include <QTimer>
#include <QWidget>
#include <iostream>

#ifdef _WIN32
static HANDLE pipe = nullptr;
#endif

class JsInterface : public QObject {
    Q_OBJECT
 public:

    /// get keys
    Q_INVOKABLE QString get_keys() const {
#ifdef _WIN32
        if (pipe != INVALID_HANDLE_VALUE) {
            // The read operation will block until there is data to read
            wchar_t buffer[1000];
            DWORD numBytesRead = 0;
            BOOL result        = ReadFile(
                pipe,
                buffer,                 // the data from the pipe will be put here
                999 * sizeof(wchar_t),  // number of bytes allocated
                &numBytesRead,          // this will store number of bytes actually read
                NULL                    // not using overlapped IO
            );

            if (result) {
                buffer[numBytesRead / sizeof(wchar_t)] = '\0';  // null terminate the string
                std::string res(std::begin(buffer), std::end(buffer));
                return res.data();
            }
        }

        return "{ \"a_button\": 0, \"ctrl_button\": 0, \"d_button\": 0, \"e_button\": 0, \"q_button\": 0, \"s_button\": 0, \"shift_button\": 0, \"space_button\": 0, \"w_button\": 0 }";
#else
        return "{ \"a_button\": 89, \"ctrl_button\": 72, \"d_button\": 5, \"e_button\": 2, \"q_button\": 2, \"s_button\": 4, \"shift_button\": 300, \"space_button\": 20, \"w_button\": 135 }";
#endif
    }
};

#include "webui.moc"

static QString qWebChannelJs() {
    QFile file(QStringLiteral(":/res/thirdparty/qwebchannel.js"));
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);
    return in.readAll();
}

Webui::Webui(QWidget* parent) : QWidget(parent) {
    WebUiHandler::registerUrlScheme();

    this->m_profile = std::make_unique<QWebEngineProfile>();

    this->m_handler = std::make_unique<WebUiHandler>();
    this->m_profile->installUrlSchemeHandler(WebUiHandler::schemeName, this->m_handler.get());

    this->m_jsinterface = new JsInterface();
    this->m_channel     = std::make_unique<QWebChannel>();
    this->m_channel->registerObject(QString("JsInterface"), this->m_jsinterface);

    this->m_page = std::make_unique<QWebEnginePage>(m_profile.get());
    this->m_page->setWebChannel(this->m_channel.get());
    this->m_page->load(WebUiHandler::mainUrl);

    this->m_view = std::make_unique<QWebEngineView>();
    this->m_view->setPage(this->m_page.get());
    this->m_view->setContextMenuPolicy(Qt::NoContextMenu);
    this->m_view->resize(500, 600);

#ifdef _WIN32
    pipe = CreateFileW(
        L"\\\\.\\pipe\\SportTech",
        GENERIC_READ,  // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
#endif

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&Webui::paintLoop));
    timer->start(std::chrono::milliseconds(50 * refresh_rate));
}

Webui::~Webui() {
#ifdef _WIN32
    CloseHandle(pipe);
#endif
    delete this->m_jsinterface;
}

void Webui::paintLoop() {
    this->m_view->page()->runJavaScript(qWebChannelJs());

    QString code2 = QStringLiteral(
        R"DELIM(
        window.webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
        const cpp = channel.objects.JsInterface;

        cpp.get_keys().then((data) => { update_pie_most_pressed(chart, JSON.parse(data)); });
    });
    )DELIM");
    this->m_view->page()->runJavaScript(code2);
}

void Webui::show() {
    this->m_view->show();
}
