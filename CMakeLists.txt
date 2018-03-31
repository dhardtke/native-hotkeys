cmake_minimum_required(VERSION 3.6)
project(hotkeys)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# switch subsystem to windows to avoid showing console window
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif ()

set(SOURCE_FILES main.cpp)

# build a static library: currently disabled
# SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# SET(BUILD_SHARED_LIBRARIES OFF)
# SET(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(hotkeys ${SOURCE_FILES})
