include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${INCLUDES}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if (MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
else ()
    target_include_directories(${PROJECT_NAME} PRIVATE ${URING_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${URING_LINK_LIBRARIES})
endif ()