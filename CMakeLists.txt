cmake_minimum_required(VERSION 4.3.0)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b") # 4.3

project(try-opengl VERSION 0.1.0 LANGUAGES C CXX)

# seems import std doesn't work on windows yet...
set(CMAKE_CXX_STANDARD 26) # 23+ Required
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Optional
# set(CMAKE_CXX_MODULE_STD ON) # Required


add_subdirectory(external/glfw)
add_subdirectory(external/fmt)

list(APPEND CMAKE_PREFIX_PATH external/glbinding/build/dist)
find_package(glbinding REQUIRED)

add_executable(try-opengl src/main.cpp)
target_link_libraries(try-opengl PUBLIC glbinding::glbinding glfw fmt::fmt)
target_precompile_headers(try-opengl PUBLIC
    <glbinding/gl/gl.h>
    <glbinding/glbinding.h>
    # <print>
)

# set(ENV{VULKAN_SDK} "D:/usr/VulkanSDK/1.4.341.1")
# # list(APPEND CMAKE_PREFIX_PATH D:/usr/VulkanSDK/)
# find_package(Vulkan REQUIRED)
# # add_executable(example-vulkan src/example-vulkan.cpp)
# # target_link_libraries(example-vulkan PUBLIC glfw Vulkan::Vulkan)

