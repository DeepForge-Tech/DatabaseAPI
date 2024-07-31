cmake_minimum_required(VERSION 3.18)

project(ServicePostgresql)

add_executable(ServicePostgresql src/main.cpp)

target_link_libraries(ServicePostgresql DatabaseAPI_static jsoncpp_static)