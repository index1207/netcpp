#include "gtest/gtest.h"
#include "net/ip_address.hpp"

TEST(IpAddrses, tryParse)
{
    net::ip_address addr;
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", &addr), true);
}

TEST(IpAddrses, tryParse_failure)
{
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", nullptr), false);
}

TEST(IpAddress, toString)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
}

TEST(IpAddress, operator_equal_lvalue)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
    EXPECT_EQ(addr == net::ip_address::loopback, true);
}

TEST(IpAddress, operator_equal_rvalue)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
    EXPECT_EQ(net::ip_address::loopback == std::move(addr), true);
}
