cmake_minimum_required (VERSION 3.16.0)
project (byteray)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_subdirectory(src)

set(BITRAY_BUILD_TESTS OFF CACHE BOOL "Build the bitray tests")
if (BITRAY_BUILD_TESTS)
    add_subdirectory(tests)
endif()

set(BITRAY_BUILD_DOCS OFF CACHE BOOL "Build the bitray documentation")
if (BITRAY_BUILD_DOCS)
    add_subdirectory(docs)
endif()