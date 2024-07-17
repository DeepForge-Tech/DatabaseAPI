include(FetchContent)

# jsoncpp
# FetchContent_Declare(jsoncpp
# GIT_REPOSITORY https://github.com/DeepForge-Technology/jsoncpp.git
# GIT_TAG master
# )

# FetchContent_MakeAvailable(jsoncpp)
if (USE_SQLITE)
# SQLite
FetchContent_Declare(SQLite
  GIT_REPOSITORY https://github.com/DeepForge-Technology/SQLite.git
  GIT_TAG master
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SQLite)
endif()
# fmt
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/DeepForge-Technology/fmt.git
  GIT_TAG master
)
# if(NOT fmt_POPULATED)
#   FetchContent_Populate(fmt)

#   add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR} EXCLUDE_FROM_ALL)
# endif()
FetchContent_MakeAvailable(fmt)

# libpqxx
if (USE_POSTGRESQL)
FetchContent_Declare(libpqxx
  GIT_REPOSITORY https://github.com/DeepForge-Tech/libpqxx.git
  GIT_TAG master
)
# if(NOT libpqxx_POPULATED)
#   FetchContent_Populate(libpqxx)

#   add_subdirectory(${libpqxx_SOURCE_DIR} ${libpqxx_BINARY_DIR} EXCLUDE_FROM_ALL)
# endif()
FetchContent_MakeAvailable(libpqxx)
endif()