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

target_link_libraries(glfw)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add main executable
file(GLOB LIB_SOURCES
    "src/*.cpp"
    "src/**/*.cpp"
)
# add_executable(main ${MAIN_SOURCES})
add_library(cridgeon-gl-basic ${LIB_SOURCES})

# Link kalman library to main executable
target_link_libraries(cridgeon-gl-basic glfw OpenGL::GL ${CMAKE_DL_LIBS})

# Include directories for main
target_include_directories(cridgeon-gl-basic PRIVATE
    ${CMAKE_SOURCE_DIR}/glfw/deps
)
