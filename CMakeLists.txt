cmake_minimum_required(VERSION 3.13)

project(codecrafters-kafka LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(kafka
    src/main.cpp

    src/metadata/cluster_metadata.cpp

    src/network/server.cpp

    src/protocol/file_descriptor.cpp
    src/protocol/ireadable.cpp
    src/protocol/iwritable.cpp
)
target_include_directories(kafka PRIVATE include)
