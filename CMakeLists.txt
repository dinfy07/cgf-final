cmake_minimum_required(VERSION 3.20)
project(SolarSystemExplorer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

include(FetchContent)

# 1. Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
    GIT_SHALLOW TRUE
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# 2. Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw glm)

add_library(platform INTERFACE)
target_include_directories(platform INTERFACE include)
target_compile_definitions(platform INTERFACE GLFW_INCLUDE_NONE GL_SILENCE_DEPRECATION)
target_link_libraries(platform INTERFACE glfw OpenGL::GL glm::glm)

add_executable(solar_explorer src/main.cpp src/renderer.cpp)
target_include_directories(solar_explorer PRIVATE third_party)
target_link_libraries(solar_explorer PRIVATE platform)

# Cross-platform warning flags (MSVC vs GCC/Clang)
if(MSVC)
    target_compile_options(solar_explorer PRIVATE /W4)
else()
    target_compile_options(solar_explorer PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(solar_explorer PRIVATE SOLAR_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
