cmake_minimum_required(VERSION 3.16)

# Ensure subprojects (like yaml-cpp) compatible with newer CMake
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE INTERNAL "")

project(FreeWake LANGUAGES CXX C)

# Global C++ standard requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit compiled binaries into a dedicated bin/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Third-party dependencies
include(FetchContent)

# Eigen3 linear algebra dependency
find_package(Eigen3 3.3 QUIET CONFIG)
if (NOT Eigen3_FOUND)
    find_package(Eigen3 3.3 QUIET)
endif()

if (NOT TARGET Eigen3::Eigen)
    set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
    set(BUILD_TESTING OFF CACHE INTERNAL "")
    set(EIGEN_BUILD_PKGCONFIG OFF CACHE INTERNAL "")
    FetchContent_Declare(
        Eigen3
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Eigen3)
endif()

# yaml-cpp dependency
set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "Disable yaml-cpp tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Disable yaml-cpp tools")
set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build yaml-cpp as static library")

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0
)
FetchContent_MakeAvailable(yaml-cpp)

# FreeWake executable target
add_executable(FreeWake
    src/Main_PerfCode2018.cpp
    src/Config.cpp
    include/Config.hpp
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

# Link libraries
target_link_libraries(FreeWake PRIVATE yaml-cpp Eigen3::Eigen)

# Link math library on UNIX systems if required
if (UNIX AND NOT APPLE)
    target_link_libraries(FreeWake PRIVATE m)
endif()

# Backward-compatible target alias
add_executable(freewake ALIAS FreeWake)

