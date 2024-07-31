cmake_minimum_required(VERSION 3.18)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# UnitTest
add_executable(DatabaseAPI_UnitTest src/UnitTest/UnitTest.cpp src/UnitTest/SQLite.cpp src/UnitTest/ServicePostgresql.cpp )

target_link_libraries(
    DatabaseAPI_UnitTest
    DatabaseAPI_static
    GTest::gtest_main
)

gtest_discover_tests(DatabaseAPI_UnitTest)
add_test(NAME DatabaseAPI_UnitTest 
  COMMAND DatabaseAPI_UnitTest)