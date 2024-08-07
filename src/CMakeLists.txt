include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build
add_library(netcpp STATIC ${SOURCE_FILES})
add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(netcpp PUBLIC
        $<BUILD_INTERFACE:${INCLUDES}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)