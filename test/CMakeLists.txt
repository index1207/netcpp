file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(BINARY_NAME "unitTest")

option(gtest_disable_pthreads ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "/MT")
endif()

add_executable(${BINARY_NAME} ${TEST_FILES} ${SOURCE_FILES})

target_include_directories(${BINARY_NAME} PRIVATE ${INCLUDES})

target_link_libraries(${BINARY_NAME} PUBLIC gtest)

if (MSVC)
    target_link_libraries(${BINARY_NAME} PUBLIC ws2_32)
else ()
    target_link_libraries(${BINARY_NAME} PUBLIC uring)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "--coverage")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
    endif ()
endif ()
