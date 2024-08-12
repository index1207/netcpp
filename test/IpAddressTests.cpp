#include "gtest/gtest.h"
#include "net/IpAddress.hpp"

TEST(IpAddrses, tryParse)
{
    net::IpAddress addr;
    EXPECT_EQ(net::IpAddress::tryParse("127.0.0.1", &addr), true);
}

TEST(IpAddrses, tryParse_failure)
{
    EXPECT_EQ(net::IpAddress::tryParse("127.0.0.1", nullptr), false);
}

TEST(IpAddress, toString)
{
    auto addr = net::IpAddress::Loopback;
    EXPECT_EQ(addr.toString(), "127.0.0.1");
}

TEST(IpAddress, operator_equal_lvalue)
{
    auto addr = net::IpAddress::Loopback;
    EXPECT_EQ(addr.toString(), "127.0.0.1");
    EXPECT_EQ(addr == net::IpAddress::Loopback, true);
}

TEST(IpAddress, operator_equal_rvalue)
{
    auto addr = net::IpAddress::Loopback;
    EXPECT_EQ(addr.toString(), "127.0.0.1");
    EXPECT_EQ(net::IpAddress::Loopback == std::move(addr), true);
}
