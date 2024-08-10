#include "net/Native.hpp"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
    if (!net::Native::initialize())
        exit(EXIT_FAILURE);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
