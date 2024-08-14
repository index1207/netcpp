#include "gtest/gtest.h"
#include "net/ip_address.hpp"

TEST(IpAddrses, tryParse)
{
    net::ip_address addr;
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", &addr), true);
}

TEST(ip_address, tryParse_failure)
{
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", nullptr), false);
}

TEST(ip_address, toString)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
}

TEST(ip_address, operator_equal_lvalue)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
    EXPECT_EQ(addr == net::ip_address::loopback, true);
}

TEST(ip_address, operator_equal_rvalue)
{
    auto addr = net::ip_address::loopback;
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
    EXPECT_EQ(net::ip_address::loopback == std::move(addr), true);
}
