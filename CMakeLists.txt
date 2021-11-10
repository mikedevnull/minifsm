cmake_minimum_required(VERSION 3.19)
project(minifsm VERSION 0.0.1)

add_library(minifsm INTERFACE)
target_include_directories(minifsm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(minifsm INTERFACE cxx_std_17)

add_subdirectory(tests)