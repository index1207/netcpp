cmake_minimum_required(VERSION 3.10)

project(netcpp VERSION 0.3 DESCRIPTION "C++ Network library.")
set(CMAKE_CXX_STANDARD 20)

include(GNUInstallDirs)

set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(EXPORT_NAMESPACE "${PROJECT_NAME}::")
set(CMAKE_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
set(CMAKE_EXPORT_FILE_BASENAME "${PROJECT_NAME}Export.cmake")

set(SOURCE_FILES
        src/IoSystem.cpp
        src/Dns.cpp
        src/Endpoint.cpp
        src/Exception.cpp
        src/Context.cpp
        src/IpAddress.cpp
        src/Socket.cpp
        src/Native.cpp
)

string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")

add_compile_definitions(NETCPP_BUILD)

add_library(netcpp STATIC ${SOURCE_FILES})

add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(netcpp PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(netcpp PUBLIC
        $<BUILD_INTERFACE:${INCLUDES}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

file(GLOB_RECURSE HEADERS "${INCLUDES}/*.h" "${INCLUDES}/*.hxx" "${INCLUDES}/*.hpp")
string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
foreach(headerFile ${HEADERS})
    get_filename_component(headerFileParentDir "${headerFile}" DIRECTORY)
    file(RELATIVE_PATH headerFileRelParentDir "${INCLUDES}" "${headerFileParentDir}")

    install(FILES "${headerFile}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${headerFileRelParentDir}"
    )
endforeach()

install(TARGETS netcpp
        EXPORT netcpp
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export(TARGETS "${PROJECT_NAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        FILE "${CMAKE_EXPORT_FILE_BASENAME}"
        EXPORT_LINK_INTERFACE_LIBRARIES)

install(EXPORT "${PROJECT_NAME}"
        FILE "${CMAKE_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}")

