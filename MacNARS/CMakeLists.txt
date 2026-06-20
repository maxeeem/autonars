cmake_minimum_required(VERSION 3.15)
project(MacNARS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Objective-C++ for Metal integration
enable_language(OBJCXX)

# Find Apple frameworks
find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(METAL_FRAMEWORK Metal)
find_library(QUARTZ_CORE_FRAMEWORK QuartzCore)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.mm
    src/NarseseParser.cpp
    src/Memory.cpp
    src/Introspection.cpp
)

# Executable
add_executable(MacNARS ${SOURCES})

# Link frameworks
target_link_libraries(MacNARS
    ${FOUNDATION_FRAMEWORK}
    ${METAL_FRAMEWORK}
    ${QUARTZ_CORE_FRAMEWORK}
)
