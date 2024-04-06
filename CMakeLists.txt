cmake_minimum_required(VERSION 3.27)
project(gpu_raytracer)

find_package(glfw3 REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(gpu_raytracer main.cpp third-party/gl.h third-party/gl.c
        third-party/khrplatform.h
        src/Vertex.h
        src/Vertex.cpp
        src/Shader.cpp
        src/Shader.h
        src/Window.h
)

target_link_libraries(gpu_raytracer glfw)
#target_include_directories(gpu_raytracer PUBLIC "${PROJECT_SOURCE_DIR}/include/glad" "${PROJECT_SOURCE_DIR}/include/KHR")
