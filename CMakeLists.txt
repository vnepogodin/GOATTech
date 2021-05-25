cmake_minimum_required(VERSION 3.12)

##
## PROJECT
## name and version
##
project(overlay
        VERSION 0.6.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "BUILD: ${CMAKE_BUILD_TYPE}")


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

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2 /fp:precise /D_NDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /Od")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -flto -D_NDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
endif()

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /LTCG:INCREMENTAL")
#   find_library(HIDAPI_LIBRARY NAMES hidapi hidapi.dll PATHS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/hidapi)
endif()

#add_subdirectory(thirdparty/JoyShockLibrary)

add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

##
## Target
##
set(GUI_TYPE WIN32)
add_executable(${PROJECT_NAME} ${GUI_TYPE}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/logger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/thirdparty/json.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/overlay.h ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/overlay_mouse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vnepogodin/mainwindow.h ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/overlay.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/overlay_mouse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/forms/overlay.ui ${CMAKE_CURRENT_SOURCE_DIR}/forms/overlay_mouse.ui

    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/overlay.qrc
    )

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Svg ${HIDAPI_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Qt5Widgets_INCLUDES})
