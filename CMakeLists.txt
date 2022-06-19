cmake_minimum_required(VERSION 3.21)
project(list)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)

set(TESTS_HELPERS
        tests-helpers/element.h
        tests-helpers/element.cpp
        tests-helpers/fault-injection.h
        tests-helpers/fault-injection.cpp)

add_executable(tests tests.cpp list.h ${TESTS_HELPERS})

if (NOT MSVC)
  target_compile_options(tests PRIVATE -Wall -Wno-sign-compare -pedantic)
endif()

option(USE_SANITIZERS "Enable to build with undefined,leak and address sanitizers" OFF)
if (USE_SANITIZERS)
  message(STATUS "Enabling sanitizers...")
  target_compile_options(tests PUBLIC -fsanitize=address,undefined,leak -fno-sanitize-recover=all)
  target_link_options(tests PUBLIC -fsanitize=address,undefined,leak)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(STATUS "Enabling libc++...")
  target_compile_options(tests PUBLIC -stdlib=libc++)
  target_link_options(tests PUBLIC -stdlib=libc++)
endif()

if (CMAKE_BUILD_TYPE MATCHES "Debug")
  message(STATUS "Enabling _GLIBCXX_DEBUG...")
  target_compile_options(tests PUBLIC -D_GLIBCXX_DEBUG)
endif()

target_link_libraries(tests GTest::gtest GTest::gtest_main)
