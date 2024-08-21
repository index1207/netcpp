#include "gtest/gtest.h"

#include "net/socket.hpp"
#include "net/dns.hpp"
#include "net/context.hpp"

#include <future>
#include <format>

#define TEST_ENDPOINT net::endpoint(net::ip_address::loopback, 8888)

using namespace std::chrono_literals;

TEST(socket, open)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    net::socket s2(net::protocol::udp);
    EXPECT_EQ(s2.is_open(), true);
}

TEST(socket, close)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);

    sock.close();
    EXPECT_EQ(sock.is_open(), false);
}

TEST(socket, create)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);

    sock.create();
    EXPECT_EQ(sock.is_open(), true);
}

TEST(socket, getHandle)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);
    EXPECT_EQ(sock.get_handle(), INVALID_SOCKET);

    sock.create(net::protocol::tcp);
    EXPECT_NE(sock.get_handle(), INVALID_SOCKET);
}

TEST(socket, set_handle)
{
    net::socket s1(net::protocol::tcp), s2;
    EXPECT_EQ(s1.is_open(), true);
    EXPECT_EQ(s2.is_open(), false);

    auto handle = s1.get_handle();

    s2.set_handle(handle);
    EXPECT_EQ(s2.get_handle(), handle);
}

TEST(socket, setOption_failure)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);
    EXPECT_EQ(sock.set_option(net::options::level::socket, net::option::broadcast, false), false);
}

TEST(socket, sync_connect)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::dns::get_host_entry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_EQ(endpoint.to_string(),
                 std::format("{}:{}", entry.address_list[0].to_string(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);
}

TEST(socket, async_connect)
{
	net::socket sock(net::protocol::tcp);
	EXPECT_EQ(sock.is_open(), true);

	auto httpsPort = 443;
	auto example = "www.example.com";
	auto entry = net::dns::get_host_entry(example);
	EXPECT_GT(entry.address_list.size(), 0);

	net::endpoint endpoint(entry.address_list[0], httpsPort);
	EXPECT_EQ(endpoint.to_string(),
			  std::format("{}:{}", entry.address_list[0].to_string(), httpsPort));

	std::atomic<std::optional<bool>> flag;
	net::context ctx;
	ctx.endpoint = endpoint;
	ctx.completed = [&flag](net::context*, bool success) {
		flag = success;
	};
	EXPECT_EQ(sock.connect(&ctx), true);

	while (!flag.load().has_value()) {}
	EXPECT_EQ(flag.load(), true);
}

TEST(socket, bind)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_reuse_address(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto localEndpoint = sock.get_local_endpoint();
    auto remoteEndpoint = sock.get_remote_endpoint();
    EXPECT_EQ(localEndpoint.has_value(), true);
    EXPECT_EQ(remoteEndpoint.has_value(), false);
    EXPECT_EQ(localEndpoint.value(), TEST_ENDPOINT);
}

TEST(socket, listen)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_reuse_address(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
    EXPECT_EQ(sock.listen(), true);
}

TEST(socket, server_get_local_endpoint)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_reuse_address(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto localEndpoint = sock.get_local_endpoint();
    EXPECT_EQ(localEndpoint.has_value(), true);
    EXPECT_EQ(localEndpoint.value(), TEST_ENDPOINT);
}

TEST(socket, server_get_remote_endpoint)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_reuse_address(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto remoteEndpoint = sock.get_remote_endpoint();
    EXPECT_EQ(remoteEndpoint.has_value(), false);
}

TEST(socket, sync_disconnect)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::dns::get_host_entry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_EQ(endpoint.to_string(),
              std::format("{}:{}", entry.address_list[0].to_string(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);

    sock.disconnect();
    EXPECT_EQ(sock.get_remote_endpoint().has_value(), false);
}

TEST(socket, async_disconnect)
{
	net::socket sock(net::protocol::tcp);
	EXPECT_EQ(sock.is_open(), true);
	EXPECT_EQ(sock.set_reuse_address(true), true);
	EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
	EXPECT_EQ(sock.listen(), true);

	std::atomic<std::optional<bool>> flag;
	auto ctx = new net::context;
	ctx->completed = [&](net::context* ctx, bool success) {
		ctx->completed = [&flag](net::context* ctx, bool success) {
			flag = success;
			//delete ctx;
		};
		ctx->accept_socket->disconnect(ctx);
	};
	EXPECT_EQ(sock.accept(ctx), true);

	std::this_thread::sleep_for(100ms);

	auto client = std::async(std::launch::async, [] {
		net::socket sock(net::protocol::tcp);
		EXPECT_EQ(sock.is_open(), true);
		EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);
	});

	while(!flag.load().has_value()) {}
	EXPECT_EQ(flag.load(), true);
}

TEST(socket, sync_accept)
{
    auto server = std::async(std::launch::async, [] {
        net::socket sock(net::protocol::tcp);
        EXPECT_EQ(sock.is_open(), true);
        EXPECT_EQ(sock.set_reuse_address(true), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);
        EXPECT_EQ(sock.accept().is_open(), true);
    });
    std::this_thread::sleep_for(100ms);
    auto client = std::async(std::launch::async, [] {
        net::socket sock(net::protocol::tcp);
        EXPECT_EQ(sock.is_open(), true);
        EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);
    });

	server.get();
	client.get();
}

TEST(socket, async_accept)
{
	net::socket sock(net::protocol::tcp);
	EXPECT_EQ(sock.is_open(), true);
	EXPECT_EQ(sock.set_reuse_address(true), true);
	EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
	EXPECT_EQ(sock.listen(), true);

	std::atomic<std::optional<bool>> flag;
	auto ctx = new net::context;
	ctx->completed = [&flag](net::context* ctx, bool success) {
		flag = success;
		//delete ctx;
	};
	EXPECT_EQ(sock.accept(ctx), true);

	std::this_thread::sleep_for(100ms);

	auto client = std::async(std::launch::async, [] {
		net::socket sock(net::protocol::tcp);
		EXPECT_EQ(sock.is_open(), true);
		EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);
	});

	while(!flag.load().has_value()) {}
	EXPECT_EQ(flag.load(), true);
}

TEST(socket, sync_send)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::dns::get_host_entry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_EQ(endpoint.to_string(),
              std::format("{}:{}", entry.address_list[0].to_string(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);

    char buffer[] = "Hello";
    EXPECT_GE(sock.send(buffer), 0);
}

TEST(socket, async_send)
{
	net::socket sock(net::protocol::tcp);
	EXPECT_EQ(sock.is_open(), true);

	auto httpsPort = 443;
	auto example = "www.example.com";
	auto entry = net::dns::get_host_entry(example);
	EXPECT_GT(entry.address_list.size(), 0);

	net::endpoint endpoint(entry.address_list[0], httpsPort);
	EXPECT_EQ(endpoint.to_string(),
			  std::format("{}:{}", entry.address_list[0].to_string(), httpsPort));
	EXPECT_EQ(sock.connect(endpoint), true);

	std::atomic<std::optional<bool>> flag;
	std::string data = "Hello, World!";
	auto ctx = new net::context;
	ctx->completed = [&flag](net::context* ctx, bool success) {
		flag = success && ctx->length > 0;
		//delete ctx;
	};
	ctx->buffer = data;
	EXPECT_EQ(sock.send(ctx), true);

	while(!flag.load().has_value()) {}
	EXPECT_EQ(flag.load(), true);
}

TEST(socket, sync_sendto)
{
	net::socket server(net::protocol::udp);
	EXPECT_EQ(server.is_open(), true);
	EXPECT_EQ(server.bind(TEST_ENDPOINT), true);

	net::socket client(net::protocol::udp);
	EXPECT_EQ(client.is_open(), true);

	char buffer[] = "Hello";
	EXPECT_GE(client.send(buffer, TEST_ENDPOINT), 0);
}

TEST(socket, sync_receive)
{
    auto server = std::async(std::launch::async, [] {
        net::socket sock(net::protocol::tcp);
        EXPECT_EQ(sock.is_open(), true);
        EXPECT_EQ(sock.set_reuse_address(true), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);

        auto client = sock.accept();
        EXPECT_EQ(client.is_open(), true);

        char buffer[] = "hello";
        EXPECT_GE(client.send(buffer), 0);
    });
    std::this_thread::sleep_for(100ms);
    auto client = std::async(std::launch::async, [] {
        net::socket sock(net::protocol::tcp);
        EXPECT_EQ(sock.is_open(), true);
        EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);

        char buffer[16] = { 0, };
        EXPECT_GE(sock.receive(buffer), 0);
    });

	server.get();
	client.get();
}

TEST(socket, async_receive)
{
	net::socket sock(net::protocol::tcp);
	EXPECT_EQ(sock.is_open(), true);
	EXPECT_EQ(sock.set_reuse_address(true), true);
	EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
	EXPECT_EQ(sock.listen(), true);

	net::context ctx;
	ctx.completed = [](net::context* ctx, bool) {
		char buffer[] = "hello";
		EXPECT_GE(ctx->accept_socket->send(buffer), 0);
	};
	EXPECT_EQ(sock.accept(&ctx), true);

	std::this_thread::sleep_for(100ms);

	std::atomic<std::optional<bool>> flag;
	auto fut = std::async(std::launch::async, [&] {
		net::socket client(net::protocol::tcp);
		EXPECT_EQ(client.is_open(), true);
		EXPECT_EQ(client.connect(TEST_ENDPOINT), true);

		auto ctx = new net::context;
		char buffer[16] = { 0, };
		ctx->buffer = buffer;
		ctx->completed = [&flag](net::context* ctx, bool success) {
			flag = true;
		};
		EXPECT_EQ(client.receive(ctx), true);
	});

	while (!flag.load().has_value()) {};
	EXPECT_EQ(flag.load(), true);
}

TEST(socket, sync_receive_from)
{
    auto server = std::async(std::launch::async, [] {
        net::socket server(net::protocol::udp);
        EXPECT_EQ(server.is_open(), true);
        EXPECT_EQ(server.set_reuse_address(true), true);
        EXPECT_EQ(server.bind(TEST_ENDPOINT), true);

        net::endpoint clientEndpoint;
        char buffer[16] { 0, };
        EXPECT_GE(server.receive(buffer, clientEndpoint), 0);
    });
    std::this_thread::sleep_for(100ms);
    auto client = std::async(std::launch::async, [] {
        net::socket client(net::protocol::udp);
        EXPECT_EQ(client.is_open(), true);

        char buffer[] = "Hello";
        EXPECT_GE(client.send(buffer, TEST_ENDPOINT), 0);
    });
}

TEST(socket, disable_blocking)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_blocking(false), true);

    char buffer[16] { 0, };
    EXPECT_EQ(sock.receive(buffer), SOCKET_ERROR);
}

TEST(socket, disable_blocking_failure)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);
    EXPECT_EQ(sock.set_blocking(false), false);
}

TEST(socket, set_linger)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_linger({.enabled = true, .time = 0}), true);
}

TEST(socket, set_broadcast)
{
    net::socket sock(net::protocol::udp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_broadcast(true), true);
}

TEST(socket, set_reuse_address)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_reuse_address(true), true);
}

TEST(socket, set_no_delay)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_no_delay(true), true);
}

TEST(socket, set_ttl)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_ttl(255), true);
}

TEST(socket, set_send_buffer)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_send_buffer(1024), true);
}

TEST(socket, set_receive_buffer)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
    EXPECT_EQ(sock.set_receive_buffer(1024), true);
}

TEST(socket, is_open)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
}

TEST(socket, get_option)
{
    net::socket sock(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);

    int value{};
    EXPECT_EQ(sock.get_option(net::options::level::socket, net::option::send_buffer, value), true);
}

TEST(socket, get_option_failure)
{
    net::socket sock;
    EXPECT_EQ(sock.is_open(), false);

    int value{};
    EXPECT_EQ(sock.get_option(net::options::level::socket, net::option::send_buffer, value), false);
}

TEST(socket, operator_assignment_lvalue)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    net::socket s2;
    s2 = s1;
    EXPECT_EQ(s2.is_open(), true);
}

TEST(socket, operator_assignment_rvalue)
{
    net::socket sock;
    sock = net::socket(net::protocol::tcp);
    EXPECT_EQ(sock.is_open(), true);
}

TEST(socket, operator_equal_lvalue)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    net::socket s2 = s1;
    EXPECT_EQ(s2.is_open(), true);
    EXPECT_EQ(s1 == s2, true);
}

TEST(socket, operator_equal_rvalue)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    net::socket s2;
    s2.set_handle(s1.get_handle());
    EXPECT_EQ(s2.is_open(), true);
    EXPECT_EQ(s1 == std::move(s2), true);
}

TEST(socket, constructor_lvalue)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    auto handle = s1.get_handle();
    EXPECT_NE(handle, INVALID_SOCKET);

    net::socket s2(s1);
    EXPECT_EQ(s2.get_handle(), handle);
}

TEST(socket, constructor_rvalue)
{
    net::socket s1(net::protocol::tcp);
    EXPECT_EQ(s1.is_open(), true);

    auto handle = s1.get_handle();
    EXPECT_NE(handle, INVALID_SOCKET);

    net::socket s2(std::move(s1));
    EXPECT_EQ(s2.get_handle(), handle);
}
