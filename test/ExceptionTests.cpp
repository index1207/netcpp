#include "gtest/gtest.h"
#include "net/exception.hpp"
#include "net/socket.hpp"

TEST(Exception, what)
{
    try {
        net::socket sock;
        EXPECT_EQ(sock.is_open(), false);
        EXPECT_EQ(sock.listen(), false);
        throw net::network_exception("listen()");
    }
    catch (net::network_exception& e)
    {
        EXPECT_NE(e.what(), "");
    }
}

TEST(Exception, getCode)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);
    EXPECT_EQ(sock.listen(), false);
    net::network_exception err("listen()");
    EXPECT_NE(err.get_code(), 0);
}