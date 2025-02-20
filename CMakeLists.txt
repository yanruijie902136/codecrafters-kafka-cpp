cmake_minimum_required(VERSION 3.13)

project(codecrafters-kafka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(kafka ${SOURCE_FILES})
target_include_directories(kafka PRIVATE src)
