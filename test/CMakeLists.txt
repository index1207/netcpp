file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(BINARY_NAME "googletest")

option(gtest_disable_pthreads ON)

add_executable(${BINARY_NAME} ${TEST_FILES} ${SOURCE_FILES})

target_include_directories(${BINARY_NAME} PRIVATE ${INCLUDES})

target_link_libraries(${BINARY_NAME} gtest gtest_main netcpp)
if (MSVC)
    target_link_libraries(${BINARY_NAME} ws2_32)
else ()
    set(CMAKE_CXX_FLAGS --coverage)
endif ()