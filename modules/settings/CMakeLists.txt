cmake_minimum_required(VERSION 3.12)

##
## PROJECT
## name and version
##
project(GOATTech-settings
        VERSION 0.8.5)

##
## INCLUDE
##
include(FindThreads)

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets Multimedia REQUIRED)

##
## CONFIGURATION
##
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
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
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /arch:AVX2 /fp:precise")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /Od")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math -flto")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
endif()

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /LTCG:INCREMENTAL")
endif()


##
## Target
##
set(GUI_TYPE WIN32)
add_executable(${PROJECT_NAME} ${GUI_TYPE}
    src/utils.hpp
    src/settings.hpp src/settings.cpp
    src/settings.ui

    src/main.cpp settings.qrc
    )

add_compile_definitions(${CMAKE_THREAD_DEFS_INIT})
add_compile_options(${CMAKE_CXX_FLAGS} ${CMAKE_THREAD_DEFS_INIT})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Multimedia ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ${Qt5Widgets_INCLUDES})
