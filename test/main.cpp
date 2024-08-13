#include "gtest/gtest.h"
#include "net/native.hpp"

int main(int argc, char **argv) {
    EXPECT_EQ(net::native::initialize(), true);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
