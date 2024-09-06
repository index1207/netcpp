include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build
if (NETCPP_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE NETCPP_BUILD_SHARED)
else ()
    add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE NETCPP_STATIC)
endif ()

add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${INCLUDES}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if (MSVC)
    if (NETCPP_BUILD_SHARED)
        target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
    else ()
        target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
    endif ()
else ()
    target_include_directories(${PROJECT_NAME} PRIVATE ${URING_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${URING_LINK_LIBRARIES})
endif ()