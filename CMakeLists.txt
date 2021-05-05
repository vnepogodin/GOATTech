cmake_minimum_required(VERSION 3.12)
project(overlay)

##
## INCLUDE
##
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg REQUIRED)


##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    find_library(HIDAPI_LIBRARY NAMES hidapi hidapi.dll PATHS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/hidapi)
else()
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lhid -lhidapi-hidraw -lhidapi-libusb")
endif()

add_subdirectory(thirdparty/JoyShockLibrary)


##
## Target
##
SET(GUI_TYPE WIN32)
add_executable(${PROJECT_NAME} ${GUI_TYPE}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/thirdparty/json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/overlay.h ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/overlay_mouse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/overlay.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/overlay_mouse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/forms/overlay.ui ${CMAKE_CURRENT_SOURCE_DIR}/forms/overlay_mouse.ui

    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/overlay.qrc
    )

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Svg ${HIDAPI_LIBRARY} JoyShockLibrary ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/thirdparty/JoyShockLibrary ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/hidapi ${Qt5Widgets_INCLUDES})
