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
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES uuid)
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.12.0
    GIT_PROGRESS ON
)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.86.0
    GIT_PROGRESS ON
)
FetchContent_MakeAvailable(json)
FetchContent_MakeAvailable(Boost)

target_include_directories(${LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(${LIB_NAME}
    PRIVATE
        nlohmann_json::nlohmann_json
        Boost::uuid
)

# Set library properties
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
) 