cmake_minimum_required(VERSION 3.6)
project(hotkeys)

include(CheckCXXCompilerFlag)

if (CHECK_CXX_COMPILER_USED1)

elseif ("${CMAKE_CXX_COMPILER_ID}x" STREQUAL "MSVCx")
    # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "-O2")
elseif ("${CMAKE_CXX_COMPILER_ID}x" STREQUAL "Intelx")
    # using Intel C++
else ()
    # GCC or Clang
    # C++11
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1Z)
    CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
    CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if (COMPILER_SUPPORTS_CXX17)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    elseif (COMPILER_SUPPORTS_CXX1Z)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    elseif (COMPILER_SUPPORTS_CXX14)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    elseif (COMPILER_SUPPORTS_CXX1Y)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
    elseif (COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif (COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else ()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif (COMPILER_SUPPORTS_CXX17)

    if (CHECK_CXX_COMPILER_USED2)
    elseif ("${CMAKE_CXX_COMPILER_ID}x" MATCHES "Clangx")
        # using Clang
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-global-constructors")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    elseif ("${CMAKE_CXX_COMPILER_ID}x" STREQUAL "GNUx")
        # using GCC
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2 -mwindows")
        set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
    endif (CHECK_CXX_COMPILER_USED2)
endif (CHECK_CXX_COMPILER_USED1)

set(SOURCE_FILES main.cpp)

add_executable(hotkeys ${SOURCE_FILES})
