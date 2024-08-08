cmake_minimum_required(VERSION 3.10)

project(netcpp VERSION 0.3 DESCRIPTION "C++ Network library." LANGUAGES CXX)

include(GNUInstallDirs)

# Platforms
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
else()
    add_compile_definitions(_LIBCPP_ENABLE_EXPERIMENTAL)
endif(MSVC)
add_compile_definitions(NETCPP_BUILD)

# googletest
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG        v1.15.2
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    include_directories(${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include)
endif()

FetchContent_MakeAvailable(googletest)
enable_testing()

# Initialize
set(CMAKE_CXX_STANDARD 20)
set(EXPORT_NAMESPACE "${PROJECT_NAME}::")
set(CMAKE_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
set(CMAKE_EXPORT_FILE_BASENAME "${PROJECT_NAME}Export.cmake")

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
set(INCLUDES "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(src)
add_subdirectory(test)

string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")

# INSTALL
file(GLOB_RECURSE HEADERS "${INCLUDES}/*.h" "${INCLUDES}/*.hxx" "${INCLUDES}/*.hpp")
string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
foreach(headerFile ${HEADERS})
    get_filename_component(headerFileParentDir "${headerFile}" DIRECTORY)
    file(RELATIVE_PATH headerFileRelParentDir "${INCLUDES}" "${headerFileParentDir}")

    install(FILES "${headerFile}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${headerFileRelParentDir}"
    )
endforeach()

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export(TARGETS "${PROJECT_NAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        FILE "${CMAKE_EXPORT_FILE_BASENAME}"
        EXPORT_LINK_INTERFACE_LIBRARIES)

install(EXPORT "${PROJECT_NAME}"
        FILE "${CMAKE_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}")

