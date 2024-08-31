#include "gtest/gtest.h"
#include "net/exception.hpp"
#include "net/socket.hpp"

TEST(exception, what)
{
    try {
        net::socket sock;
        EXPECT_EQ(sock.is_open(), false);
        EXPECT_EQ(sock.listen(), false);
        throw net::exception("listen()");
    }
    catch (net::exception& e)
    {
        EXPECT_NE(e.what(), "");
    }
}

TEST(exception, get_code)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);
    EXPECT_EQ(sock.listen(), false);
    net::exception err("listen()");
    EXPECT_NE(err.get_code(), 0);
}