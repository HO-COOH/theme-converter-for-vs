# Executable CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Define the executable name
set(EXE_NAME ThemeConverter)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${EXE_NAME} ${SOURCES})

# Link with the static library
target_link_libraries(${EXE_NAME} 
    PRIVATE 
        ThemeConverterCppLib
)

# Set executable properties
set_target_properties(${EXE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
) 