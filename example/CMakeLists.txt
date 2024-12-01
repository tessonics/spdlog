# Copyright(c) 2019 spdlog authors Distributed under the MIT License (http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 3.14)
project(spdlog_example CXX)

find_package(spdlog REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE spdlog $<$<BOOL:${MINGW}>:ws2_32>)
