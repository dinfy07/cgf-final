cmake_minimum_required(VERSION 3.20)
project(SolarSystemExplorer VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(OpenGL REQUIRED)
include(FetchContent)

find_package(glfw3 3.3 QUIET CONFIG)
if(NOT TARGET glfw)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(glfw)
endif()

find_package(glm CONFIG QUIET)
if(NOT TARGET glm::glm)
  set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    glm_archive
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
  )
  FetchContent_MakeAvailable(glm_archive)
endif()

add_library(platform INTERFACE)
target_include_directories(platform INTERFACE include)
target_compile_definitions(platform INTERFACE GLFW_INCLUDE_NONE GL_SILENCE_DEPRECATION)
target_link_libraries(platform INTERFACE glfw OpenGL::GL glm::glm)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui)
add_library(imgui_lib STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_lib PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui_lib PUBLIC platform)

add_executable(solar_explorer src/main.cpp src/mesh.cpp src/texture.cpp src/scene.cpp src/renderer.cpp src/ui.cpp)
target_include_directories(solar_explorer PRIVATE third_party)
target_link_libraries(solar_explorer PRIVATE platform imgui_lib)
if(MSVC)
  target_compile_options(solar_explorer PRIVATE /W4)
else()
  target_compile_options(solar_explorer PRIVATE -Wall -Wextra -Wpedantic)
endif()
target_compile_definitions(solar_explorer PRIVATE SOLAR_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
enable_testing()
add_executable(core_tests tests/core_tests.cpp src/scene.cpp)
target_include_directories(core_tests PRIVATE include)
target_link_libraries(core_tests PRIVATE glm::glm)
add_test(NAME hierarchy_camera_and_geometry COMMAND core_tests)
