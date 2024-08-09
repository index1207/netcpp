#include "gtest/gtest.h"

#include "net/Socket.hpp"
#include "net/Dns.hpp"

#include <format>

TEST(Socket, open)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
}

TEST(Socket, close)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    sock.close();
    EXPECT_EQ(sock.isOpen(), false);
}

TEST(Socket, create)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);

    sock.create();
    EXPECT_EQ(sock.isOpen(), true);
}

TEST(Socket, getHandle)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);
    EXPECT_EQ(sock.getHandle(), INVALID_SOCKET);

    sock.create(net::Protocol::Tcp);
    EXPECT_NE(sock.getHandle(), INVALID_SOCKET);
}

TEST(Socket, setHandle)
{
    net::Socket s1(net::Protocol::Tcp), s2;
    EXPECT_EQ(s1.isOpen(), true);
    EXPECT_EQ(s2.isOpen(), false);

    auto handle = s1.getHandle();

    s2.setHandle(handle);
    EXPECT_EQ(s2.getHandle(), handle);
}

TEST(Socket, sync_connect)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::Dns::getHostEntry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::Endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_STREQ(endpoint.toString().c_str(),
                 std::format("{}:{}", entry.address_list[0].toString(), httpsPort).c_str());
    EXPECT_EQ(sock.connect(endpoint), true);
}

TEST(Socket, sync_disconnect)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::Dns::getHostEntry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::Endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_STREQ(endpoint.toString().c_str(),
                 std::format("{}:{}", entry.address_list[0].toString(), httpsPort).c_str());
    EXPECT_EQ(sock.connect(endpoint), true);

    sock.disconnect();
    EXPECT_EQ(sock.getRemoteEndpoint().has_value(), false);
}

TEST(Socket, bind)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(net::Endpoint(net::IpAddress::Loopback, 1234)), true);
}

TEST(Socket, listen)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(net::Endpoint(net::IpAddress::Loopback, 1234)), true);
    EXPECT_EQ(sock.listen(), true);
}

TEST(Socket, getLocalEndpoint)
{
    net::Socket sock(net::Protocol::Tcp);
    net::Endpoint endpoint(net::IpAddress::Loopback, 1234);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(endpoint), true);
    EXPECT_EQ(sock.getLocalEndpoint().has_value(), true);
    EXPECT_EQ(sock.getLocalEndpoint().value(), endpoint);
}

TEST(Socket, constructor_lvalue)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    auto handle = s1.getHandle();
    EXPECT_NE(handle, INVALID_SOCKET);

    net::Socket s2(s1);
    EXPECT_EQ(s2.getHandle(), handle);
}

TEST(Socket, constructor_rvalue)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    auto handle = s1.getHandle();
    EXPECT_NE(handle, INVALID_SOCKET);

    net::Socket s2(std::move(s1));
    EXPECT_EQ(s2.getHandle(), handle);
}