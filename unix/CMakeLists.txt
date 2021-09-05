cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

##
## PROJECT
## name and version
##
project(overlay
        VERSION 0.9.3)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
message(STATUS "BUILD: ${CMAKE_BUILD_TYPE}")


##
## INCLUDE
##
include(CompilerWarnings)
include(EnableCcache)
include(ClangTidy)
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg Multimedia REQUIRED)

##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(UNIX AND CMAKE_GENERATOR STREQUAL "Ninja")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fcolor-diagnostics ${CMAKE_C_FLAGS}")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fdiagnostics-color ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fdiagnostics-color ${CMAKE_C_FLAGS}")
  endif()
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -flto")

add_library(project_warnings INTERFACE)
set_project_warnings(project_warnings)

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

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
if(UNIX)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Svg Qt5::Multimedia uiohook frozen::frozen ${CMAKE_THREAD_LIBS_INIT})
else()
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Svg Qt5::Multimedia uiohook ${CMAKE_THREAD_LIBS_INIT})
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
