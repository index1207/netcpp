#include "gtest/gtest.h"
#include "net/context.hpp"

TEST(context, set_buffer_ptr)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.set_buffer(buffer, 0, 128);
}

TEST(context, set_buffer_span)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.set_buffer(buffer);
}

TEST(context, add_data_ptr)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.add_data(buffer, 0, sizeof(buffer));
}

TEST(context, add_data_span)
{
    char buffer[128] = "";
    net::context ctx;
    ctx.add_data(buffer);
}

TEST(context, set_buffer_ptr_failure)
{
    net::context ctx;

    char buffer[128] = "";
    ctx.add_data(buffer);

    try
    {
        ctx.set_buffer(buffer, 0, 128);
    }
    catch (std::exception& e)
    {
        EXPECT_NE(e.what(), "");
        return;
    }
    EXPECT_EQ(true, false);
}

TEST(context, set_buffer_span_failure)
{
    net::context ctx;

    char buffer[128] = "";
    ctx.add_data(buffer);

    try
    {
        ctx.set_buffer(buffer);
    }
    catch (std::exception& e)
    {
        EXPECT_NE(e.what(), "");
        return;
    }
    EXPECT_EQ(true, false);
}
