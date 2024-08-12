file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(BINARY_NAME "unitTest")

option(gtest_disable_pthreads ON)

add_executable(${BINARY_NAME} ${TEST_FILES} ${SOURCE_FILES})

target_include_directories(${BINARY_NAME} PRIVATE ${INCLUDES})

target_link_libraries(${BINARY_NAME} PUBLIC gtest netcpp::netcpp)
if (MSVC)
    target_link_libraries(${BINARY_NAME} PUBLIC ws2_32)
else ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "--coverage")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    endif ()
endif ()
