cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##
## PROJECT
## name and version
##
project(overlay
        VERSION 0.9.3)

##
## INCLUDE
##
include(StandardProjectSettings)
include(CompilerWarnings)
include(EnableCcache)
include(ClangTidy)
include(FindThreads)
include(FetchContent)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg Multimedia REQUIRED)

##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -flto")

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

add_subdirectory(../modules/settings ${CMAKE_CURRENT_BINARY_DIR}/modules/settings)

add_subdirectory(thirdparty)

##
## Target
##
add_executable(${PROJECT_NAME} WIN32
    include/vnepogodin/buffer.hpp
    include/vnepogodin/uiohook_helper.hpp src/uiohook_helper.cpp
    include/vnepogodin/input_data.hpp src/input_data.cpp
    include/vnepogodin/recorder.hpp
    include/vnepogodin/logger.hpp
    include/vnepogodin/utils.hpp
    include/vnepogodin/overlay.hpp src/overlay.cpp
    include/vnepogodin/overlay_mouse.hpp src/overlay_mouse.cpp
    include/vnepogodin/overlay_keyboard.hpp src/overlay_keyboard.cpp
    include/vnepogodin/mainwindow.hpp src/mainwindow.cpp

    src/main.cpp ../assets/overlay.qrc
    )

option(ENABLE_UNITY "Enable Unity builds of projects" OFF)
if(ENABLE_UNITY)
  # Add for any project you want to apply unity builds for
  set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

if(UNIX)
target_link_libraries(${PROJECT_NAME} PRIVATE project_warnings project_options Qt5::Widgets Qt5::Svg Qt5::Multimedia uiohook frozen::frozen nlohmann_json::nlohmann_json HTTPRequest ${CMAKE_THREAD_LIBS_INIT})
else()
target_link_libraries(${PROJECT_NAME} PRIVATE project_warnings project_options Qt5::Widgets Qt5::Svg Qt5::Multimedia uiohook nlohmann_json::nlohmann_json HTTPRequest ${CMAKE_THREAD_LIBS_INIT})
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(UNIX)
add_custom_target(run
    COMMAND ./${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(run-settings
    COMMAND ./GOATTech-settings
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules/settings)
endif()
