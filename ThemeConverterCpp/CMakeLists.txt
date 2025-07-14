cmake_minimum_required(VERSION 3.16)

# Project name and version
project(ThemeConverterCpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type to Debug by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add subdirectories
add_subdirectory(ThemeConverterCppLib)
add_subdirectory(ThemeConverter)
