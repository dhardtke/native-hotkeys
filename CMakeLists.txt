cmake_minimum_required(VERSION 3.6)
project(hotkeys)

set(SOURCE_FILES main.cpp lib/INIReader.h lib/Error.h)

add_executable(hotkeys WIN32 ${SOURCE_FILES})

set_target_properties(hotkeys PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")