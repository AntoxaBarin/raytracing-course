cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
project(engine)

cmake_policy(SET CMP0072 NEW)
cmake_policy(SET CMP0074 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

add_subdirectory(source/glm)

set(TARGET_NAME "${PROJECT_NAME}")
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE 
    source/main.cpp
    source/io.cpp 
    source/io.hpp 
    source/scene.hpp 
    source/primitive.hpp 
)

add_executable(${TARGET_NAME} source/main.cpp)

target_link_libraries(${TARGET_NAME} PUBLIC glm)
target_compile_definitions(${TARGET_NAME} PUBLIC -DPROJECT_ROOT="${PROJECT_ROOT}")
