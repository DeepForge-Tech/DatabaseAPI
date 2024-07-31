cmake_minimum_required(VERSION 3.18)
project(DatabaseAPI)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_CXX_STANDARD 20)

include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BINARY_DIR "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(USE_SQLITE "Use SQLite library for DatabaseAPI" ON)
option(USE_POSTGRESQL "Use Postgresql library for DatabaseAPI" OFF)
option(USE_SERVICE_POSTGRESQL "Use CURL for send requests to service of postgresql" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadDependecies.cmake)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG
            "/DWIN32 /D_WINDOWS /EHsc /WX /wd4355 /wd4251 /wd4250 /wd4996 /FS"
            CACHE STRING "Debug compiler flags" FORCE
    )
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_CXX_FLAGS} -I /usr/local/include -I ../../include -DCURL_STATICLIB -Bstatic")
endif ()

set(SOURCE_FILES
    src/Advanced.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# fmt
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/DeepForge-Technology/fmt.git
  GIT_TAG master
  GIT_SHALLOW 1
)
# if(NOT fmt_POPULATED)
#   FetchContent_Populate(fmt)

#   add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
# endif()
FetchContent_MakeAvailable(fmt)
message(STATUS "USE_SQLITE is set to ${USE_SQLITE}")
if (USE_SQLITE)
    # SQLite
    FetchContent_Declare(SQLite
      GIT_REPOSITORY https://github.com/DeepForge-Technology/SQLite.git
      GIT_TAG master
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(SQLite)
    list(APPEND SOURCE_FILES src/SQLiteDB.cpp)
    list(APPEND LIBRARIES SQLite)
endif()

# libpqxx
if (USE_POSTGRESQL)
FetchContent_Declare(libpqxx
  GIT_REPOSITORY https://github.com/DeepForge-Tech/libpqxx.git
  GIT_TAG master
  GIT_SHALLOW 1
)
# if(NOT libpqxx_POPULATED)
#   FetchContent_Populate(libpqxx)

#   add_subdirectory(${libpqxx_SOURCE_DIR} ${libpqxx_BINARY_DIR} EXCLUDE_FROM_ALL)
# endif()
FetchContent_MakeAvailable(libpqxx)
endif()

if(USE_SERVICE_POSTGRESQL)
  if(WIN32)
    set(USE_WINDOWS_SSPI ON)
    set(USE_SCHANNEL ON)
    set(CMAKE_USE_WINSSL ON)
  endif()
  find_package(OpenSSL REQUIRED)
  # jsoncpp
  FetchContent_Declare(jsoncpp
          GIT_REPOSITORY https://github.com/DeepForge-Technology/jsoncpp.git
          GIT_TAG master
  )
  # if(NOT jsoncpp_POPULATED)
  #   FetchContent_Populate(jsoncpp)
  #   add_subdirectory(${jsoncpp_SOURCE_DIR} ${jsoncpp_BINARY_DIR})
  # endif()
  FetchContent_MakeAvailable(jsoncpp)

  FetchContent_Declare(libcurl
    GIT_REPOSITORY https://github.com/DeepForge-Technology/curl.git
    GIT_TAG master
    GIT_SHALLOW 1
  )
  # if(NOT libcurl_POPULATED)
  #   FetchContent_Populate(libcurl)

  #   add_subdirectory(${libcurl_SOURCE_DIR} ${libcurl_BINARY_DIR} EXCLUDE_FROM_ALL)
  # endif()
  FetchContent_MakeAvailable(libcurl)
  list(APPEND SOURCE_FILES src/ServicePostgresqlDB.cpp)
  list(APPEND LIBRARIES libcurl_static jsoncpp_static OpenSSL::SSL OpenSSL::Crypto)
endif()

add_library(DatabaseAPI SHARED ${SOURCE_FILES})

add_library(DatabaseAPI_static STATIC ${SOURCE_FILES})

list(APPEND LIBRARIES fmt)

target_link_libraries(DatabaseAPI ${LIBRARIES})
target_link_libraries(DatabaseAPI_static ${LIBRARIES})
# Building examples
# add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/example/example.cpp)
# target_link_libraries(example DatabaseAPI)

# Tests
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

# Examples
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/example)