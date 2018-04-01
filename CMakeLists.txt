cmake_minimum_required(VERSION 3.6)
project(hotkeys)

set(CMAKE_CXX_FLAGS "-O2")
set(SOURCE_FILES main.cpp)

add_executable(hotkeys WIN32 ${SOURCE_FILES})

set_target_properties(hotkeys PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")