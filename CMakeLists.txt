cmake_minimum_required(VERSION 3.10)

project(netcpp VERSION 0.3 DESCRIPTION "C++ Network library.")
set(CMAKE_CXX_STANDARD 20)

include_directories(include/net)

add_library(netcpp SHARED
        src/PCH.cpp
        src/IoSystem.cpp
        src/Dns.cpp
        src/Endpoint.cpp
        src/Exception.cpp
        src/Context.cpp
        src/IpAddress.cpp
        src/netcpp.cpp
        src/Socket.cpp
        src/Native.cpp)

target_include_directories(netcpp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(netcpp ws2_32)

target_precompile_headers(netcpp
        PRIVATE
        src/PCH.h)

install(TARGETS netcpp
        EXPORT netcpp_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(EXPORT netcpp_targets
        FILE netcpp-targets.cmake
        NAMESPACE netcpp::
        DESTINATION share/netcpp)