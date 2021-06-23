cmake_minimum_required(VERSION 3.12)

##
## PROJECT
## name and version
##
project(overlay
        VERSION 0.8.5)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "BUILD: ${CMAKE_BUILD_TYPE}")


##
## INCLUDE
##
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Svg Multimedia REQUIRED)

##
## OPTIONS
##
#option(ENABLE_SETTINGS "Enable GOATTech Settings" ON)

##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(CMAKE_GENERATOR STREQUAL "Ninja")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fcolor-diagnostics ${CMAKE_C_FLAGS}")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fdiagnostics-color ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-fdiagnostics-color ${CMAKE_C_FLAGS}")
  endif()
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -flto -D_NDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

#add_subdirectory(thirdparty/JoyShockLibrary)

add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

#if(ENABLE_SETTINGS)
add_subdirectory(modules/settings)
#endif()
#add_subdirectory(modules/webui)

##
## Target
##
add_executable(${PROJECT_NAME}
    include/vnepogodin/recorder.hpp
    include/vnepogodin/logger.hpp
    include/vnepogodin/utils.hpp
    include/vnepogodin/thirdparty/json.hpp
    include/vnepogodin/overlay.h src/overlay.cpp
    include/vnepogodin/overlay_mouse.h src/overlay_mouse.cpp
    forms/mainwindow.ui
    include/vnepogodin/mainwindow.hpp src/mainwindow.cpp
    forms/overlay.ui forms/overlay_mouse.ui

    src/main.cpp overlay.qrc
    )

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Svg Qt5::Multimedia ${HIDAPI_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${Qt5Widgets_INCLUDES})

add_custom_target(run
    COMMAND ./${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(run-settings
    COMMAND ./GOATTech-settings
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules/settings)
