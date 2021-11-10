Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1
)

FetchContent_MakeAvailable(Catch2)

add_executable(minifsm_tests catch_main.cpp typelist_tests.cpp)
target_link_libraries(minifsm_tests PRIVATE minifsm Catch2::Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(minifsm_tests)