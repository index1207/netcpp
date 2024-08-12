#include "gtest/gtest.h"
#include "net/Endpoint.hpp"

TEST(Endpoint, constructor)
{
    net::Endpoint endpoint(net::IpAddress::Loopback, 8080);
    EXPECT_EQ(endpoint.toString(), "127.0.0.1:8080");
}

TEST(Endpoint, getAddress)
{
    net::Endpoint endpoint(net::IpAddress::Loopback, 8080);
    EXPECT_EQ(endpoint.getAddress().toString(), "127.0.0.1");
}

TEST(Endpoint, setAddress)
{
    auto addr = net::IpAddress::Loopback;
    net::Endpoint endpoint;
    endpoint.setAddress(addr);
    EXPECT_EQ(endpoint.getAddress(), addr);
}

TEST(Endpoint, getPort)
{
    u_short port = 8080;
    net::Endpoint endpoint(net::IpAddress::Loopback, port);
    EXPECT_EQ(endpoint.getPort(), port);
}

TEST(Endpoint, setPort)
{
    u_short port = 8080;
    net::Endpoint endpoint;
    endpoint.setPort(port);
    EXPECT_EQ(endpoint.getPort(), port);
}

TEST(Endpoint, toString)
{
    net::Endpoint endpoint(net::IpAddress::Loopback, 8080);
    EXPECT_EQ(endpoint.toString(), "127.0.0.1:8080");
}

TEST(Endpoint, parse)
{
    u_short port = 8080;
    sockaddr_in addr {};
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = port;

    auto endpoint = net::Endpoint::parse(addr);
    EXPECT_EQ(endpoint.getPort(), port);
}

TEST(Endpoint, tryParse)
{
    net::Endpoint endpoint;
    EXPECT_EQ(net::Endpoint::tryParse("127.0.0.1:8080", &endpoint), true);
}

TEST(Endpoint, operator_equla_lvalue)
{
    auto addr = net::IpAddress::Loopback;
    auto port = 8080;
    net::Endpoint ep1(addr, port), ep2(addr, port);
    EXPECT_EQ(ep1 == ep2, true);
}

TEST(Endpoint, operator_equla_rvalue)
{
    auto addr = net::IpAddress::Loopback;
    auto port = 8080;
    net::Endpoint ep1(addr, port), ep2(addr, port);
    EXPECT_EQ(ep1 == std::move(ep2), true);
}
