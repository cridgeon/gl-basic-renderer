cmake_minimum_required(VERSION 3.10)
project(kalman-test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Enable debug symbols for Debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# Add GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add main library
file(GLOB LIB_SOURCES
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
)
add_library(cridgeon-gl-basic ${LIB_SOURCES})

# Link libraries
target_link_libraries(cridgeon-gl-basic PUBLIC 
    glfw 
    OpenGL::GL 
    ${CMAKE_DL_LIBS}
)

# Include directories
target_include_directories(cridgeon-gl-basic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/deps
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
)

add_custom_command(
    TARGET cridgeon-gl-basic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
)