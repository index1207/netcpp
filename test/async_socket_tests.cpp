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

    // Client
    std::this_thread::sleep_for(1000ms);
    std::jthread client([] {
        net::socket client(net::protocol::tcp);
        EXPECT_EQ(client.is_open(), true);
        EXPECT_EQ(client.connect(TEST_ENDPOINT), true);
    });

    EXPECT_EQ(result.load(), true);
}

//TEST(async_socket, send)
//{
//    auto entry = net::dns::get_host_entry("www.example.com");
//    EXPECT_GT(entry.address_list.size(), 0);
//
//    net::async_socket sock(net::protocol::tcp);
//    EXPECT_EQ(sock.is_open(), true);
//
//    std::atomic<std::optional<bool>> result;
//    net::context conn;
//    conn.endpoint = net::endpoint(entry.address_list[0], 443);
//    conn.completed = [&result](net::context*, bool success) {
//        result.store(success);
//    };
//    EXPECT_EQ(sock.connect(&conn), true);
//
//    while (!result.load().has_value()) {}
//    EXPECT_EQ(result.load(), true);
//
//    result.store(std::nullopt);
//
//    net::context send_ctx;
//    send_ctx.completed = [&result](net::context*, bool success) {
//        result.store(success);
//    };
//    EXPECT_EQ(send_ctx.create_buffer(0x10000), true); // Page size
//    EXPECT_EQ(sock.send(&send_ctx), true);
//
//    while (!result.load().has_value()) {}
//    EXPECT_EQ(result.load(), true);
//}

#endif
