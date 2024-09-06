#include "gtest/gtest.h"
#include "net/ip_address.hpp"

TEST(ip_address, try_parse)
{
    net::ip_address addr;
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", &addr), true);
}

TEST(ip_address, try_parse_failure)
{
    EXPECT_EQ(net::ip_address::try_parse("127.0.0.1", nullptr), false);
}

TEST(ip_address, parse)
{
    auto addr = net::ip_address::parse(INADDR_LOOPBACK);
    EXPECT_EQ(addr.to_string(), "127.0.0.1");
}

TEST(ip_address, to_string)
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
