cmake_minimum_required(VERSION 3.18)
project(DatabaseAPI)
cmake_policy(SET CMP0135 NEW)
set(CMAKE_CXX_STANDARD 20)
include(GNUInstallDirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadDependecies.cmake)
set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(BUILD_SHARED_LIBS ON)
option(USE_SQLITE "Use SQLite database" ON)
option(USE_POSTGRESQL  "Use PostgreSQL database" ON)

set(SOURCE_FILES
  src/Advanced.cpp
)



message(STATUS "USE_SQLITE is set to ${USE_SQLITE}")
if (USE_SQLITE)
    list(APPEND SOURCE_FILES src/SQLiteDB.cpp)
endif()

message(STATUS "USE_SQLITE is set to ${USE_POSTGRESQL}")
if (USE_POSTGRESQL)
    list(APPEND SOURCE_FILES src/PostgresqlDB.cpp)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG
      "/DWIN32 /D_WINDOWS /EHsc /WX /wd4355 /wd4251 /wd4250 /wd4996 /MT"
      CACHE STRING "Debug compiler flags" FORCE
    )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")  
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -I /usr/local/include -I ../../include")
endif()

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
else()
    add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
endif()

set(LIBRARIES SQLite)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test)
set(TEST_SOURCE_FILES
  test/DatabaseTest.cpp
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