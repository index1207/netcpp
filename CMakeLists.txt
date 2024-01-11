cmake_minimum_required(VERSION 3.9)

project(netcpp VERSION 1.0.0 DESCRIPTION "C++ Network library.")
set(CMAKE_CXX_STANDARD 20)

include_directories(include/net)

add_library(netcpp STATIC)

target_sources(netcpp PRIVATE
        src/PCH.cpp
        src/IoSystem.cpp
        src/Dns.cpp
        src/Endpoint.cpp
        src/Exception.cpp
        src/Context.cpp
        src/IpAddress.cpp
        src/netcpp.cpp
        src/Socket.cpp
        src/Native.cpp
)

target_include_directories(netcpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(netcpp ws2_32)

target_precompile_headers(netcpp
        PRIVATE
        src/PCH.h)

project(example)

add_executable(example example/connect.cpp)

target_link_libraries(example netcpp)

project(test)

add_executable(test test/test.cpp)

target_link_libraries(test netcpp ws2_32)