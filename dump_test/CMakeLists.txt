cmake_minimum_required(VERSION 3.10)
project(dump_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

enable_testing()

set(TESTS
    tests/simple_test.cpp
)

set(SRC
    main.cpp
)

add_executable(dump_test ${SRC} ${TESTS})

target_link_libraries(dump_test ${GTEST_LIBRARIES} pthread)