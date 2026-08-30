cmake_minimum_required(VERSION 3.14)
project(gbpp)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(gbpp_core src/cpu.cpp)
target_include_directories(gbpp_core PUBLIC include)

add_executable(gbpp src/main.cpp)
target_link_libraries(gbpp PRIVATE gbpp_core)

enable_testing()
add_executable(unit_tests tests/test_main.cpp)

target_link_libraries(unit_tests PRIVATE 
  gbpp_core
  GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)