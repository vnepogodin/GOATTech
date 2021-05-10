#include <vnepogodin/utils.hpp>
#include <vnepogodin/overlay_gamepad.h>
#include <ui_overlay_gamepad.h>

static constexpr auto MAX_PLAYERS = 12;

Overlay::Overlay(QWidget* parent) : QWidget(parent), ui(new Ui::Overlay) {
    ui->setupUi(this);
    connect(this, &Overlay::mouseDoubleClickEvent, this,
        &Overlay::connectGamepad);
    connectGamepad();
}

void Overlay::paintEvent(QPaintEvent*) {
    // Initialize renderer with base asset
    QSvgRenderer* renderer;

    if (gamepadConnected)
        renderer = new QSvgRenderer(QString(":/assets/dualshock_black/base.svg"));
    else
        renderer = new QSvgRenderer(QString(":/assets/dualshock_black/disconnected.svg"));

    renderer->setAspectRatioMode(Qt::KeepAspectRatio);

    // Paint base svg on widget
    QPainter painter(this);
    renderer->render(&painter);
    renderer->setViewBox(QRectF(0, 0, this->width(), this->height()));

    if (gamepadConnected) {
        QPoint corner = locateCorner(renderer->defaultSize(), renderer->viewBox().size());
        double scale  = getScale(renderer->defaultSize(), renderer->viewBox().size());
        paintFeatures(this, corner, scale);
    }
}

void Overlay::paintFeatures(QPaintDevice* device, QPoint corner, double scale) {
    paintButtons(this, corner, scale);
    paintAxes(this, corner, scale);
    paintTouch(this, corner, scale);
}

QPoint Overlay::locateCorner(QSize defaultSize, QSize viewBox) {
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
        x      = (int)(.5 + ((double)viewBox.width() - width) / 2);
    } else if (arCoeff < 1) {
        width  = (double)viewBox.width();
        height = width * 1 / defaultAR;
        x      = 0;
        y      = (int)(.5 + ((double)viewBox.height() - height) / 2);
    } else {
        x = 0;
        y = 0;
    }

    return QPoint(x, y);
}

double Overlay::getScale(QSize defaultSize, QSize viewBox) {
    double result;
    QPoint corner = locateCorner(defaultSize, viewBox);

    // Since svg maintains aspect ratio, only height or width is needed
    double width = (double)viewBox.width() - 2 * corner.x();
    result       = width / (double)defaultSize.width();

    return result;
}

bool Overlay::connectGamepad() {
    // Get max amount connected controllers
    int handle_array[MAX_PLAYERS];
    int num_devices = JslConnectDevices();
    num_devices     = JslGetConnectedDeviceHandles(handle_array, MAX_PLAYERS);

    // Check for devices
    if (num_devices == 0) {
#ifndef _NDEBUG
        std::cout << "No controllers found" << std::endl;
#endif
        gamepadConnected = false;
        if (poll.joinable())
            poll.join();
        return false;
    }

    // Check if controller is connected; if not try to make connection to new
    // controller
    for (int i = 0; i < num_devices; i++) {
        if (this->deviceID == handle_array[i]) {
            return true;
        }
    }
#ifndef _NDEBUG
    std::cout << "Change of controllers" << std::endl;
#endif
    deviceID = handle_array[0];

    // Start polling
    gamepadConnected = false;

    if (poll.joinable())
        poll.join();

    gamepadConnected = true;
    poll             = std::thread(&Overlay::paintLoop, this);

    return true;
}

void Overlay::paintLoop() {
    while (gamepadConnected) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / refresh_rate));
    }
}
Overlay::~Overlay() {
    gamepadConnected = false;
    poll.join();
    JslDisconnectAndDisposeAll();
}
void Overlay::paintButtons(QPaintDevice* device, QPoint corner, double scale) {
    // using json = nlohmann::json;
    // const std::string filename = "assets/dualshock_black/placement.json";
    // std::ifstream reader(filename);
    // json j;
    // reader >> j;
    // reader.close();

    int buttons = JslGetButtons(deviceID);

    QPainter painter(device);
    QSvgRenderer renderer;

    std::unordered_map<int, std::pair<std::string, QPoint>> button_map = {
        {JSMASK_DOWN, {"dpad_down", {136, 255}}},
        {JSMASK_LEFT, {"dpad_left", {92, 226}}},
        {JSMASK_RIGHT, {"dpad_right", {165, 226}}},
        {JSMASK_UP, {"dpad_up", {136, 181}}},
        {JSMASK_HOME, {"home", {382, 344}}},
        {JSMASK_L, {"left_bumper", {109, 94}}},
        {JSMASK_ZL, {"left_trigger", {108, 0}}},
        {JSMASK_E, {"o_button", {682, 217}}},
        {JSMASK_R, {"right_bumper", {598, 94}}},
        {JSMASK_ZR, {"right_trigger", {597, 0}}},
        {JSMASK_W, {"square_button", {567, 217}}},
        {JSMASK_SHARE, {"share", {227, 142}}},
        {JSMASK_OPTIONS, {"options", {551, 142}}},
        {JSMASK_TOUCHPAD_CLICK, {"touchpad", {272, 122}}},
        {JSMASK_N, {"triangle_button", {629, 159}}},
        {JSMASK_S, {"x_button", {629, 276}}}};

    for (const auto& [mask, asset] : button_map) {
        if ((mask & buttons) != 0) {
            std::string filename = ":/assets/dualshock_black/" + asset.first + ".svg";

            renderer.load(QString(filename.c_str()));

            const int& width  = utils::round((double)renderer.defaultSize().width() * scale);
            const int& height = utils::round((double)renderer.defaultSize().height() * scale);

            QPoint location = QPoint(utils::round((double)asset.second.x() * scale) + corner.x(),
                utils::round((double)asset.second.y() * scale) + corner.y());

            QImage image(width, height, QImage::Format_ARGB32);
            image.fill(Qt::transparent);
            QPainter imagePainter(&image);
            renderer.render(&imagePainter);

            QPainter painter(device);
            painter.drawImage(location, image);
        }
    }
}

void Overlay::paintAxes(QPaintDevice* device, QPoint corner, double scale) {
    double offset = 20;

    std::pair<std::string, QPoint> right{"right_stick", {484, 308}};
    std::pair<std::string, QPoint> left{"left_stick", {228, 308}};
    if ((JSMASK_LCLICK & JslGetButtons(deviceID)) != 0)
        left.first = "left_stick_pressed";
    if ((JSMASK_RCLICK & JslGetButtons(deviceID)) != 0)
        right.first = "right_stick_pressed";

    QPoint location = QPoint(utils::round(((double)right.second.x() + offset * JslGetRightX(deviceID)) * scale) + corner.x(), utils::round(((double)right.second.y() - offset * JslGetRightY(deviceID)) * scale) + corner.y());

    paintAsset(right.first, location, this, corner, scale);

    location = QPoint(utils::round(((double)left.second.x() + offset * JslGetLeftX(deviceID)) * scale) + corner.x(), utils::round(((double)left.second.y() - offset * JslGetLeftY(deviceID)) * scale) + corner.y());
    paintAsset(left.first, location, this, corner, scale);

    // std::string filename = ":/assets/dualshock_black/" + asset.first + ".svg";

    // renderer.load(QString(filename.c_str()));

    // int width = utils::round((double)renderer.defaultSize().width() * scale);
    // int height = utils::round((double)renderer.defaultSize().height() * scale);

    // QPoint location = QPoint(utils::round((double)(asset.second.x() + offset) *
    // scale) + corner.x(),
    //                          utils::round((double)(asset.second.y() + offset) *
    //                          scale) + corner.y());

    // QImage image(width, height, QImage::Format_ARGB32);
    // image.fill(Qt::transparent);
    // QPainter imagePainter(&image);
    // renderer.render(&imagePainter);

    // QPainter painter(device);
    // painter.drawImage(location, image);
}
void Overlay::paintAsset(std::string name, QPoint place, QPaintDevice* device,
    QPoint corner, double scale) {
    name = ":/assets/dualshock_black/" + name + ".svg";
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

void Overlay::paintTouch(QPaintDevice* device, QPoint corner, double scale) {
    QPoint tl{269, 119};
    float height = 151, width = 262;

    TOUCH_STATE state = JslGetTouchState(deviceID);

    if (state.t0Down) {
        QPoint location = QPoint(
            utils::round(((double)tl.x() + (double)(width * state.t0X)) * scale) + corner.x(),
            utils::round(((double)tl.y() + (double)(height * state.t0Y)) * scale) + corner.y());
        paintAsset("cursor", location, this, corner, scale);
    }
    if (state.t1Down) {
        QPoint location = QPoint(
            utils::round(((double)tl.x() + (double)(width * state.t1X)) * scale) + corner.x(),
            utils::round(((double)tl.y() + (double)(height * state.t1Y)) * scale) + corner.y());
        paintAsset("cursor", location, this, corner, scale);
    }
}
