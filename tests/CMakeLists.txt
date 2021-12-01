Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  FakeIt
  GIT_REPOSITORY https://github.com/eranpeer/FakeIt.git
  GIT_TAG        2.0.9
)

FetchContent_MakeAvailable(FakeIt)
add_library(FakeIt INTERFACE)
target_include_directories(FakeIt INTERFACE ${fakeit_SOURCE_DIR}/single_header/catch)

add_executable(minifsm_tests 
    catch_main.cpp 
    transitions_tests.cpp
    transition_table_tests.cpp
    state_machine_tests.cpp
    tuple_tests.cpp
    typelist_tests.cpp)

target_link_libraries(minifsm_tests 
    PRIVATE 
    minifsm 
    Catch2::Catch2
    FakeIt)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)
catch_discover_tests(minifsm_tests)
