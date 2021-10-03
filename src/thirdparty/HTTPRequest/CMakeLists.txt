cmake_minimum_required(VERSION 3.13)

project(HTTPRequest LANGUAGES CXX)

add_library(HTTPRequest INTERFACE
    include/HTTPRequest.hpp
)

include(GNUInstallDirs)
target_include_directories(HTTPRequest
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

list(REMOVE_DUPLICATES INTERFACE_LINK_LIBRARIES)
string(REPLACE ";" " " COMPILE_LIBRARIES "${INTERFACE_LINK_LIBRARIES}")
