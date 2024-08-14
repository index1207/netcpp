#include "gtest/gtest.h"
#include "net/endpoint.hpp"

TEST(endpoint, constructor)
{
    net::endpoint endpoint(net::ip_address::loopback, 8080);
    EXPECT_EQ(endpoint.to_string(), "127.0.0.1:8080");
}

TEST(endpoint, getAddress)
{
    net::endpoint endpoint(net::ip_address::loopback, 8080);
    EXPECT_EQ(endpoint.get_address().to_string(), "127.0.0.1");
}

TEST(endpoint, setAddress)
{
    auto addr = net::ip_address::loopback;
    net::endpoint endpoint;
    endpoint.set_address(addr);
    EXPECT_EQ(endpoint.get_address(), addr);
}

TEST(endpoint, getPort)
{
    u_short port = 8080;
    net::endpoint endpoint(net::ip_address::loopback, port);
    EXPECT_EQ(endpoint.get_port(), port);
}

TEST(endpoint, setPort)
{
    u_short port = 8080;
    net::endpoint endpoint;
    endpoint.set_port(port);
    EXPECT_EQ(endpoint.get_port(), port);
}

TEST(endpoint, toString)
{
    net::endpoint endpoint(net::ip_address::loopback, 8080);
    EXPECT_EQ(endpoint.to_string(), "127.0.0.1:8080");
}

TEST(endpoint, parse)
{
    u_short port = 8080;
    sockaddr_in addr {};
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = port;

    auto endpoint = net::endpoint::parse(addr);
    EXPECT_EQ(endpoint.get_port(), port);
}

TEST(endpoint, tryParse)
{
    net::endpoint endpoint;
    EXPECT_EQ(net::endpoint::try_parse("127.0.0.1:8080", &endpoint), true);
}

TEST(endpoint, operator_equla_lvalue)
{
    auto addr = net::ip_address::loopback;
    auto port = 8080;
    net::endpoint ep1(addr, port), ep2(addr, port);
    EXPECT_EQ(ep1 == ep2, true);
}

TEST(endpoint, operator_equla_rvalue)
{
    auto addr = net::ip_address::loopback;
    auto port = 8080;
    net::endpoint ep1(addr, port), ep2(addr, port);
    EXPECT_EQ(ep1 == std::move(ep2), true);
}
