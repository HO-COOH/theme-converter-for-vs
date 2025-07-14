# Static library CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Define the library name
set(LIB_NAME ThemeConverterCppLib)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

# Create static library
add_library(${LIB_NAME} STATIC ${SOURCES} ${HEADERS})

# Set include directories
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

target_include_directories(${LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME}
    PRIVATE
        nlohmann_json::nlohmann_json
)

# Set library properties
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
) 