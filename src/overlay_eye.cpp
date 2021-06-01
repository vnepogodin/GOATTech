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

#include <vnepogodin/overlay_eye.h>
#include <vnepogodin/thirdparty/HTTPRequest.hpp>

#include <vnepogodin/logger.hpp>
#include <vnepogodin/utils.hpp>

#include <array>
#include <charconv>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QSvgRenderer>
#include <QSettings>

using namespace vnepogodin;

static inline QPoint parse_fromString(std::string& str) {
    std::regex regexp("-?\\d+%");

    // flag type for determining the matching behavior (in this case on string objects)
    std::smatch match;

    std::array<int, 3> coors;

    int i = 0;
    while (std::regex_search(str, match, regexp)) {
        std::string tmp   = match.str(0);
        const auto& value = tmp.erase((tmp.size() == 3) ? 2 : 1);
        int result        = 0;
        std::from_chars(value.data(), value.data() + value.size(), result);
        coors[i] = result;
        ++i;

        // suffix to find the rest of the string.
        str = match.suffix().str();
    }
    if (coors[1] >= 0 && coors[1] <= 5)
        coors[1] += 6;

    return {coors[0], coors[1]};
}

static inline int load_key(nlohmann::json& json, const std::string& key) {
    if (json.contains(key)) {
        if (json[key].is_string()) {
            const auto& value = json[key].get<std::string>();
            int result        = 0;
            std::from_chars(value.data(), value.data() + value.size(), result);
            return result;
        } else {
            return json[key].get<int>();
        }
    }

    return 20;
}

namespace vnepogodin {
namespace utils {
static std::vector<std::string> fromStringList(const QStringList& string_list) {
    const auto& len = string_list.size();
    std::vector<std::string> keys(len);

    for (int i = 0; i < len; ++i) {
        keys[i] = string_list[i].toStdString();
    }

    return keys;
}
static void toObject(const QSettings* const settings, nlohmann::json& obj) {
    for (const auto& _ : settings->childKeys()) {
        if (!_.size()) {
            return;
        }
        QVariant value        = settings->value(_);
        const std::string key = _.toStdString();
        switch ((QMetaType::Type)value.type()) {
        case QMetaType::Bool:
            obj[key] = value.toBool();
            break;
        case QMetaType::Int:
            obj[key] = value.toInt();
            break;
        case QMetaType::Double:
            obj[key] = value.toDouble();
            break;
        case QMetaType::QString:
            obj[key] = value.toString().toStdString();
            break;
        case QMetaType::QStringList:
            obj[key] = fromStringList(value.toStringList());
            break;
        case QMetaType::QByteArray:
            obj[key] = QString::fromUtf8(value.toByteArray().toBase64()).toStdString();
            break;
        default:
            break;
        }
    }
}

static bool isPointInsideCircle(const QPoint& point, const QPoint& circle, const int& radius) {
    const auto& d = std::sqrt(std::pow(point.x() - circle.x(), 2) + std::pow(point.y() - circle.y(), 2));

    return d <= radius;
}

static constexpr bool coors_valid(const QPoint& point) {
    return !((point.x() < 0) || (point.y() < 0));
}
};
}

Overlay_Eye::Overlay_Eye(QWidget* parent) : QWidget(parent) {
    ui->setupUi(this);

    setAttribute(Qt::WA_NativeWindow);
    connectMouse();
    QSettings settings(QSettings::UserScope);
    nlohmann::json json;
    utils::toObject(&settings, json);

    radius = load_key(json, "radius");
    centerX = load_key(json, "centerX");
    centerY = load_key(json, "centerY");
}

void Overlay_Eye::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer renderer;

    if (mouseConnected)
        renderer.load(QString(":/assets/eye/base.svg"));
    else
        renderer.load(QString(":/assets/mouse/disconnected.svg"));

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    // Paint base svg on widget
    QPainter painter(this);
    renderer.render(&painter);
    renderer.setViewBox(QRectF(0, 0, this->width(), this->height()));

    if (mouseConnected) {
        QPoint corner = locateCorner(renderer.defaultSize(), renderer.viewBox().size());
        double scale  = getScale(renderer.defaultSize(), renderer.viewBox().size());
        paintFeatures(this, corner, scale);
    }
}

void Overlay_Eye::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintTouch(this, corner, scale);
}

QPoint Overlay_Eye::locateCorner(QSize defaultSize, QSize viewBox) {
    double defaultAR = (double)defaultSize.width() / (double)defaultSize.height();
    double viewAR    = (double)viewBox.width() / (double)viewBox.height();

    // @var arCoeff lets us know which side of the svg borders the widget
    double arCoeff = viewAR / defaultAR;
    double width, height;
    int x, y;

    if (arCoeff > 1) {
        height = (double)viewBox.height();
        width  = height * defaultAR;
        y      = 0;
        x      = (int)(0.5 + ((double)viewBox.width() - width) / 2);
    } else if (arCoeff < 1) {
        width  = (double)viewBox.width();
        height = width * 1 / defaultAR;
        x      = 0;
        y      = (int)(0.5 + ((double)viewBox.height() - height) / 2);
    } else {
        x = 0;
        y = 0;
    }

    return QPoint(x, y);
}

double Overlay_Eye::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool Overlay_Eye::connectMouse() {
    // Start polling
    mouseConnected = false;

    if (poll.joinable())
        poll.join();

    mouseConnected = true;
    poll           = std::thread(&Overlay_Eye::paintLoop, this);

    return true;
}

void Overlay_Eye::paintLoop() {
    while (mouseConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate));
    }
}

Overlay_Eye::~Overlay_Eye() {
    mouseConnected = false;
    poll.join();
}

void Overlay_Eye::paintAsset(std::string name, const QPoint& place, QPaintDevice* device, const double& scale) {
    name = ":/assets/eye/" + name + ".svg";
    QSvgRenderer renderer;
    renderer.load(QString(name.c_str()));

    const int& width  = utils::round((double)renderer.defaultSize().width() * scale);
    const int& height = utils::round((double)renderer.defaultSize().height() * scale);

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter imagePainter(&image);
    renderer.render(&imagePainter);

    QPainter painter(device);
    painter.drawImage(place, image);
}

void Overlay_Eye::paintTouch(QPaintDevice* device, QPoint corner, double scale) {
    static constexpr auto URL = "http://www.headkraken.gg/soft/SkolkovoJuniorChallenge/9-10/test/";
    QPoint tl{0, 0};
    float height = 151, width = 262;
    http::Request request(URL);

    // send a get request
    const auto response = request.send("GET");
    std::string str(response.body.begin(), response.body.end());
    const auto& res = parse_fromString(str);
    if (!utils::coors_valid(res)) {
        return;
    }

    const QPoint coors{static_cast<int>((width * res.x() / 100 - 14)), static_cast<int>((height * res.y() / 100 - 14))};

    const QPoint& location = QPoint(
        (int)utils::round(((double)tl.x() + (double)((unsigned)coors.x() % (unsigned)width)) * scale) + corner.rx(),
        (int)utils::round(((double)tl.y() + (double)((unsigned)coors.y() % (unsigned)height)) * scale) + corner.ry());

    if (utils::isPointInsideCircle(res, QPoint(centerX, centerY), radius)) {
        paintAsset("green", location, device, scale - 0.5);
    } else {
        paintAsset("red", location, device, scale - 0.5);
    }
}
