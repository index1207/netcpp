file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(BINARY_NAME "unitTest")

option(gtest_disable_pthreads ON)

add_executable(${BINARY_NAME} ${TEST_FILES} ${SOURCE_FILES})

target_compile_definitions(${BINARY_NAME} PRIVATE NETCPP_STATIC)

target_include_directories(${BINARY_NAME} PRIVATE ${INCLUDES})

target_link_libraries(${BINARY_NAME} PRIVATE gtest)

if (MSVC)
    target_link_libraries(${BINARY_NAME} PRIVATE ws2_32)
else ()
    target_include_directories(${BINARY_NAME} PRIVATE ${URING_INCLUDE_DIRS})
    target_link_libraries(${BINARY_NAME} PRIVATE ${URING_LINK_LIBRARIES})

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "--coverage")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_libraries(${BINARY_NAME} PRIVATE atomic)
        set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    endif ()
endif ()
