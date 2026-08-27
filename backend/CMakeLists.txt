cmake_minimum_required(VERSION 3.24)
project(monetix LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Boost 1.83 REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

add_library(monetix_common INTERFACE)
target_include_directories(monetix_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/common)

add_subdirectory(gateway)
add_subdirectory(services)

enable_testing()
add_executable(monetix_tests tests/unit_tests.cpp)
target_link_libraries(monetix_tests PRIVATE monetix_common OpenSSL::Crypto Threads::Threads)
add_test(NAME monetix_tests COMMAND monetix_tests)
