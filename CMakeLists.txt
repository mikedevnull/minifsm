cmake_minimum_required(VERSION 3.16)
project(minifsm VERSION 0.0.1)

add_library(minifsm INTERFACE)
target_include_directories(minifsm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(minifsm INTERFACE cxx_std_17)

include(CTest)
add_subdirectory(tests)