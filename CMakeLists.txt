cmake_minimum_required(VERSION 3.16)
project(FreeWake LANGUAGES CXX C)

# Global C++ standard requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit compiled binaries into a dedicated bin/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# FreeWake executable target
add_executable(FreeWake
    src/Main_PerfCode2018.cpp
    include/FreeWakeWing.h
    include/PerfCode.h
    include/alloc.h
    include/general.h
    include/ref_frame_transform.h
    include/typedef.h
    include/vector_algebra.h
)

# Target-scoped include directories
target_include_directories(FreeWake PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Target-scoped compiler warning flags
if (MSVC)
    target_compile_options(FreeWake PRIVATE /W4)
else()
    target_compile_options(FreeWake PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link math library on UNIX systems if required
if (UNIX AND NOT APPLE)
    target_link_libraries(FreeWake PRIVATE m)
endif()

# Backward-compatible target alias
add_executable(freewake ALIAS FreeWake)

