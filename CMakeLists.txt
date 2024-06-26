cmake_minimum_required(VERSION 3.18)
project(DatabaseAPI)
cmake_policy(SET CMP0135 NEW)
set(CMAKE_CXX_STANDARD 20)
include(GNUInstallDirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadDependecies.cmake)
set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS ON)

set(SOURCE_FILES
    include/Database/DatabaseAPI.hpp
    src/DatabaseAPI.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(BUILD_SHARED_LIBS)
    add_library(DatabaseAPI SHARED ${SOURCE_FILES})
else()
    add_library(DatabaseAPI STATIC ${SOURCE_FILES})
endif()

set(LIBRARIES SQLite)

target_link_libraries(DatabaseAPI ${LIBRARIES})

# Building examples
# add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/example/example.cpp)
# target_link_libraries(example DatabaseAPI)

#Testing
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/test)
set(TEST_SOURCE_FILES
  src/test/DatabaseTest.cpp
  include/Database/DatabaseAPI.hpp
)
enable_testing()
add_executable(
  DatabaseTest
  ${TEST_SOURCE_FILES}
)
target_link_libraries(
  DatabaseTest
  DatabaseAPI
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(DatabaseTest)