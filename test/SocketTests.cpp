#include "gtest/gtest.h"

#include "net/Socket.hpp"
#include "net/Dns.hpp"
#include "net/Exception.hpp"
#include "net/Context.hpp"

#include <future>
#include <format>

#define TEST_ENDPOINT net::Endpoint(net::IpAddress::Loopback, 8080)

TEST(Socket, open)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    net::Socket s2(net::Protocol::Udp);
    EXPECT_EQ(s2.isOpen(), true);
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
    EXPECT_EQ(endpoint.toString(),
                 std::format("{}:{}", entry.address_list[0].toString(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);
}

TEST(Socket, bind)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto localEndpoint = sock.getLocalEndpoint();
    auto remoteEndpoint = sock.getRemoteEndpoint();
    EXPECT_EQ(localEndpoint.has_value(), true);
    EXPECT_EQ(remoteEndpoint.has_value(), false);
    EXPECT_EQ(localEndpoint.value(), TEST_ENDPOINT);
}

TEST(Socket, listen)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
    EXPECT_EQ(sock.listen(), true);
}

TEST(Socket, server_getLocalEndpoint)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto localEndpoint = sock.getLocalEndpoint();
    EXPECT_EQ(localEndpoint.has_value(), true);
    EXPECT_EQ(localEndpoint.value(), TEST_ENDPOINT);
}

TEST(Socket, server_getRemoteEndpoint)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto remoteEndpoint = sock.getRemoteEndpoint();
    EXPECT_EQ(remoteEndpoint.has_value(), false);
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
    EXPECT_EQ(endpoint.toString(),
              std::format("{}:{}", entry.address_list[0].toString(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);

    sock.disconnect();
    EXPECT_EQ(sock.getRemoteEndpoint().has_value(), false);
}

TEST(Socket, sync_accept)
{
    std::thread server([] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);
        EXPECT_EQ(sock.accept().isOpen(), true);
    });
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);

    server.join();
}

TEST(Socket, sync_send)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    auto httpsPort = 443;
    auto example = "www.example.com";
    auto entry = net::Dns::getHostEntry(example);
    EXPECT_GT(entry.address_list.size(), 0);

    net::Endpoint endpoint(entry.address_list[0], httpsPort);
    EXPECT_EQ(endpoint.toString(),
              std::format("{}:{}", entry.address_list[0].toString(), httpsPort));
    EXPECT_EQ(sock.connect(endpoint), true);

    char buffer[] = "Hello";
    EXPECT_GE(sock.send(buffer), 0);
}

TEST(Socket, sync_sendto)
{
    net::Socket server(net::Protocol::Udp);
    EXPECT_EQ(server.isOpen(), true);
    EXPECT_EQ(server.bind(TEST_ENDPOINT), true);

    net::Socket client(net::Protocol::Udp);
    EXPECT_EQ(client.isOpen(), true);

    char buffer[] = "Hello";
    EXPECT_GE(client.send(buffer, TEST_ENDPOINT), 0);
}

TEST(Socket, sync_receive)
{
    std::thread server([] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);
        EXPECT_EQ(sock.accept().isOpen(), true);

        std::string data = "hello";
        EXPECT_GE(sock.send(data), 0);
    });
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);

    char buffer[16] { 0, };
    EXPECT_GE(sock.receive(buffer), 0);

    server.join();
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