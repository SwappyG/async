cmake_minimum_required(VERSION 3.0.0)
set(META_PROJECT_VERSION 0.1.0)
set(META_PROJECT_NAME cpp)
project(${META_PROJECT_NAME})

add_subdirectory(src/meta)
add_subdirectory(src/async)