cmake_minimum_required (VERSION 3.12)

project(JoyShockLibrary CXX)

include(cmake/LinuxConfig.cmake)
include(cmake/WindowsConfig.cmake)

set (JSL_PLATFORM_DEPENDENCY_VISIBILITY PUBLIC)

add_subdirectory(JoyShockLibrary)
