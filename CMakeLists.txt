cmake_minimum_required(VERSION 3.8)

##
## PROJECT
## name and version
##
project(vnepogodin_static_server
        VERSION 0.0.1
        LANGUAGES CXX)

##
## MAIN_PROJECT CHECK
## determine if vnepogodin_nn is built as a subproject (using add_subdirectory) or if it is the main project
##
set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MAIN_PROJECT ON)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


##
## INCLUDE
##
include(ExternalProject)
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

##
## OPTIONS
##
if (POLICY CMP0077)
    # Allow CMake 3.13+ to override options when using FetchContent / add_subdirectory.
    cmake_policy(SET CMP0077 NEW)
endif ()

##
## CONFIGURATION
##
include(GNUInstallDirs)

set(VNEPOGODIN_SS_TARGET_NAME                ${PROJECT_NAME})

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2 /fp:precise /LTCG:INCREMENTAL")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /Od")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
endif()

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_MULTITHREADED      ON)

if((NOT MSVC) AND ((${CMAKE_SYSTEM_NAME} MATCHES "Linux") OR (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")))
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx2")
endif()

set(SS_SOURCES_DIR "${PROJECT_SOURCE_DIR}/src")

set(VNEPOGODIN_SS_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")

set(VNEPOGODIN_SS_HEADERS
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/header.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/html.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/mime_types.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/connection.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/io_service_pool.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/reply.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/request.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/request_handler.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/request_parser.hpp
    ${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}/vnepogodin/server.hpp
)
set(VNEPOGODIN_SS_SOURCES
    ${SS_SOURCES_DIR}/html.cpp
    ${SS_SOURCES_DIR}/mime_types.cpp
    ${SS_SOURCES_DIR}/connection.cpp
    ${SS_SOURCES_DIR}/io_service_pool.cpp
    ${SS_SOURCES_DIR}/reply.cpp
    ${SS_SOURCES_DIR}/request_handler.cpp
    ${SS_SOURCES_DIR}/request_parser.cpp
    ${SS_SOURCES_DIR}/server.cpp
    
    ${SS_SOURCES_DIR}/main.cpp

)

##
## TARGET
## create target and add include path
##
add_executable(${VNEPOGODIN_SS_TARGET_NAME}
    ${VNEPOGODIN_SS_SOURCES}
    ${VNEPOGODIN_SS_HEADERS}
)

target_include_directories(${VNEPOGODIN_SS_TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${VNEPOGODIN_SS_INCLUDE_BUILD_DIR}>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${VNEPOGODIN_SS_TARGET_NAME} PUBLIC cxx_std_17)

add_library(project_warnings INTERFACE)

include(${SS_SOURCES_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_compile_definitions(${VNEPOGODIN_SS_DEFS} ${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT} ${Boost_THREAD_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
