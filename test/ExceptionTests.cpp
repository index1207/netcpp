#include "gtest/gtest.h"
#include "net/Exception.hpp"
#include "net/Socket.hpp"

TEST(Exception, what)
{
    try {
        net::Socket sock;
        EXPECT_EQ(sock.isOpen(), false);
        EXPECT_EQ(sock.listen(), false);
        throw net::network_error("listen()");
    }
    catch (net::network_error& e)
    {
        EXPECT_NE(e.what(), "");
    }
}

TEST(Exception, getCode)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);
    EXPECT_EQ(sock.listen(), false);
    net::network_error err("listen()");
    EXPECT_NE(err.getCode(), 0);
}