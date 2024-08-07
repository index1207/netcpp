#include "gtest/gtest.h"
#include "net/Socket.hpp"

TEST(unittest, SocketOpenTest)
{
    net::Native::initialize();

    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
}

TEST(unittest, SocketCloseTest)
{
    net::Native::initialize();

    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    sock.close();
    EXPECT_EQ(sock.isOpen(), false);
}