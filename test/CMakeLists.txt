# CMake version
cmake_minimum_required(VERSION 2.8)
# project name
project(ringbuffer_test C CXX)

set (CMAKE_CXX_STANDARD 11)

# build: Google Test
add_subdirectory(lib/googletest/googletest)

include_directories(../src)

add_library(ringbuffer ../src/CRingBuffer.cpp)
add_executable(ringbuffer-test CRingBufferTest.cpp)
target_link_libraries(ringbuffer-test ringbuffer gtest_main)
