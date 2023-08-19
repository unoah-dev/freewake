cmake_minimum_required(VERSION 3.10)
project(freewake)

set(CMAKE_CXX_STANDARD 14)

# Linux Build
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(
            include/
            /usr/lib/x86_64-linux-gnu/
            /usr/lib/gcc/x86_64-linux-gnu/7/
    )
    link_directories(
            /usr/lib/x86_64-linux-gnu/
            /usr/lib/gcc/x86_64-linux-gnu/7/
    )
endif ()

# Windows Build
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    SET(CMAKE_MAKE_PROGRAM "mingw32-make.exe")
    include_directories(%USERPROFILE%/vcpkg/installed/x86-windows/include)
    link_directories(%USERPROFILE%/vcpkg/installed/x86-windows/lib)
endif ()

# MacOS Build
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include_directories(/usr/local/lib/)
    link_directories(/usr/local/lib)
endif ()

add_executable(freewake
        include/general.h
        include/PerfCode.h
        src/Main_PerfCode2018.cpp
)
