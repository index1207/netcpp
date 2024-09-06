#include "gtest/gtest.h"
#include "net/context.hpp"

TEST(context, set_buffer_ptr)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.set_buffer(buffer, 0, 128);
}

TEST(context, set_buffer_ptr_failure)
{
    char buffer[128] = "";
    std::vector<std::span<char>> buffer_list;
    buffer_list.push_back(buffer);
    net::context ctx;
    ctx.buffer_list = buffer_list;

    try
    {
        ctx.set_buffer(buffer, 0, 128);
    }
    catch (std::exception& e)
    {
        EXPECT_NE(e.what(), "");
    }
}

TEST(context, set_buffer_span)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.set_buffer(buffer);
}

TEST(context, set_buffer_span_failure)
{
    char buffer[128] = "";
    std::vector<std::span<char>> buffer_list;
    buffer_list.push_back(buffer);
    net::context ctx;
    ctx.buffer_list = buffer_list;

    try
    {
        ctx.set_buffer(buffer);
    }
    catch (std::exception& e)
    {
        EXPECT_NE(e.what(), "");
    }
}
