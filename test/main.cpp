#include "net/Native.hpp"
#include "gtest/gtest.h"

int main(int argc, char **argv) {
    EXPECT_EQ(net::Native::initialize(), true);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
