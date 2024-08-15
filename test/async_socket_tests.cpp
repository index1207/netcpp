#include "gtest/gtest.h"
#include "net/socket.hpp"
#include "net/context.hpp"
#include "net/dns.hpp"

#define TEST_ENDPOINT net::endpoint(net::ip_address::loopback, 5500)

using namespace std::chrono_literals;

#ifdef _WIN32
TEST(async_socket, accept)
{
    // Server
    net::async_socket server(net::protocol::tcp);
    EXPECT_EQ(server.is_open(), true);
    EXPECT_EQ(server.set_reuse_address(true), true);
    EXPECT_EQ(server.bind(TEST_ENDPOINT), true);
    EXPECT_EQ(server.listen(), true);

    std::atomic<std::optional<bool>> result;
    net::context ctx;
    ctx.completed = [&](net::context*, bool success){
        result.store(success);
    };
    ctx.accept_socket->create(net::protocol::tcp);
    EXPECT_EQ(server.accept(&ctx), true);

    // Client
    std::this_thread::sleep_for(100ms);
    std::jthread client([] {
        net::socket client(net::protocol::tcp);
        EXPECT_EQ(client.is_open(), true);
        EXPECT_EQ(client.connect(TEST_ENDPOINT), true);
    });

    while (!result.load().has_value()) {}
    EXPECT_EQ(result.load(), true);
}

TEST(async_socket, receive)
{
    net::async_socket server(net::protocol::tcp);
    EXPECT_EQ(server.is_open(), true);
    EXPECT_EQ(server.set_reuse_address(true), true);
    EXPECT_EQ(server.bind(TEST_ENDPOINT), true);
    EXPECT_EQ(server.listen(), true);

    net::context accept_ctx;
    accept_ctx.completed = [&](net::context* ctx, bool success) {
        auto client = std::move(ctx->accept_socket);

        net::context send_ctx;
        send_ctx.completed = [&](net::context *rv, bool success) {
            std::cout << rv->length << '\n';
        };
        send_ctx.create_buffer(0x10000);
        send_ctx.set_data("Hello");
        client->send(&send_ctx);
        while (true)
        {
        }
    };
    accept_ctx.accept_socket->create(net::protocol::tcp);
    EXPECT_EQ(server.accept(&accept_ctx), true);

    while (true)
    {
    }
}

#endif
