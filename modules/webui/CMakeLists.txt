cmake_minimum_required(VERSION 3.12)

##
## PROJECT
## name and version
##
project(GOATTech-charts
        VERSION 0.8.5)

##
## INCLUDE
##
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets WebEngineWidgets WebChannel REQUIRED)

##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2 /fp:precise /LTCG:INCREMENTAL")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /Od")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
endif()

##
## Target
##
SET(GUI_TYPE WIN32)
add_executable(${PROJECT_NAME} ${GUI_TYPE}
    include/vnepogodin/webuihandler.hpp src/webuihandler.cpp
    include/vnepogodin/webui.hpp src/webui.cpp

    src/main.cpp charts.qrc
    )

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::WebEngineWidgets Qt5::WebChannel ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Qt5Widgets_INCLUDES})
