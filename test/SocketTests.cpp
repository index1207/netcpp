#include "gtest/gtest.h"

#include "net/Socket.hpp"
#include "net/Dns.hpp"
#include "net/Context.hpp"

#include <future>
#include <format>

#define TEST_ENDPOINT net::Endpoint(net::IpAddress::Loopback, 5500)

using namespace std::chrono_literals;

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

TEST(Socket, setOption_failure)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);
    EXPECT_EQ(sock.setOption(net::OptionLevel::Socket, net::OptionName::Broadcast, false), false);
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
    EXPECT_EQ(sock.setReuseAddress(true), true);
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
    EXPECT_EQ(sock.setReuseAddress(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
    EXPECT_EQ(sock.listen(), true);
}

TEST(Socket, server_getLocalEndpoint)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setReuseAddress(true), true);
    EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);

    auto localEndpoint = sock.getLocalEndpoint();
    EXPECT_EQ(localEndpoint.has_value(), true);
    EXPECT_EQ(localEndpoint.value(), TEST_ENDPOINT);
}

TEST(Socket, server_getRemoteEndpoint)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setReuseAddress(true), true);
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
    auto server = std::async(std::launch::async, [] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.setReuseAddress(true), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);
        EXPECT_EQ(sock.accept().isOpen(), true);
    });
    std::this_thread::sleep_for(1s);
    auto client = std::async(std::launch::async, [] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);
    });
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
    EXPECT_EQ(server.setReuseAddress(true), true);
    EXPECT_EQ(server.bind(TEST_ENDPOINT), true);

    net::Socket client(net::Protocol::Udp);
    EXPECT_EQ(client.isOpen(), true);

    char buffer[] = "Hello";
    EXPECT_GE(client.send(buffer, TEST_ENDPOINT), 0);
}

TEST(Socket, sync_receive)
{
    auto server = std::async(std::launch::async, [] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.setReuseAddress(true), true);
        EXPECT_EQ(sock.bind(TEST_ENDPOINT), true);
        EXPECT_EQ(sock.listen(), true);

        auto client = sock.accept();
        EXPECT_EQ(client.isOpen(), true);

        char buffer[] = "hello";
        EXPECT_GE(client.send(buffer), 0);
    });
    std::this_thread::sleep_for(1s);
    auto client = std::async(std::launch::async, [] {
        net::Socket sock(net::Protocol::Tcp);
        EXPECT_EQ(sock.isOpen(), true);
        EXPECT_EQ(sock.connect(TEST_ENDPOINT), true);

        char buffer[16] = { 0, };
        EXPECT_GE(sock.receive(buffer), 0);
    });
}

TEST(Socket, sync_receive_from)
{
    auto server = std::async(std::launch::async, [] {
        net::Socket server(net::Protocol::Udp);
        EXPECT_EQ(server.isOpen(), true);
        EXPECT_EQ(server.setReuseAddress(true), true);
        EXPECT_EQ(server.bind(TEST_ENDPOINT), true);

        net::Endpoint clientEndpoint;
        char buffer[16] { 0, };
        EXPECT_GE(server.receive(buffer, clientEndpoint), 0);
    });
    std::this_thread::sleep_for(1s);
    auto client = std::async(std::launch::async, [] {
        net::Socket client(net::Protocol::Udp);
        EXPECT_EQ(client.isOpen(), true);

        char buffer[] = "Hello";
        EXPECT_GE(client.send(buffer, TEST_ENDPOINT), 0);
    });
}

TEST(Socket, disableBlocking)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setBlocking(false), true);

    char buffer[16] { 0, };
    EXPECT_EQ(sock.receive(buffer), SOCKET_ERROR);
}

TEST(Socket, disableBlocking_invalid)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);
    EXPECT_EQ(sock.setBlocking(false), false);
}

TEST(Socket, setLinger)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setLinger({ .enabled = true, .time = 0 }), true);
}

TEST(Socket, setBroadcast)
{
    net::Socket sock(net::Protocol::Udp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setBroadcast(true), true);
}

TEST(Socket, setReuseAddress)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setReuseAddress(true), true);
}

TEST(Socket, setNoDelay)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setNoDelay(true), true);
}

TEST(Socket, setTTL)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setTTL(255), true);
}

TEST(Socket, setSendBuffer)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setSendBuffer(1024), true);
}

TEST(Socket, setReceiveBuffer)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
    EXPECT_EQ(sock.setReceiveBuffer(1024), true);
}

TEST(Socket, isOpen)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
}

TEST(Socket, getOption)
{
    net::Socket sock(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);

    int value{};
    EXPECT_EQ(sock.getOption(net::OptionLevel::Socket, net::OptionName::SendBuffer, value), true);
}

TEST(Socket, getOption_failure)
{
    net::Socket sock;
    EXPECT_EQ(sock.isOpen(), false);

    int value{};
    EXPECT_EQ(sock.getOption(net::OptionLevel::Socket, net::OptionName::SendBuffer, value), false);
}

TEST(Socket, operator_assignment_lvalue)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    net::Socket s2;
    s2 = s1;
    EXPECT_EQ(s2.isOpen(), true);
}

TEST(Socket, operator_assignment_rvalue)
{
    net::Socket sock;
    sock = net::Socket(net::Protocol::Tcp);
    EXPECT_EQ(sock.isOpen(), true);
}

TEST(Socket, operator_equal_lvalue)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    net::Socket s2 = s1;
    EXPECT_EQ(s2.isOpen(), true);
    EXPECT_EQ(s1 == s2, true);
}

TEST(Socket, operator_equal_rvalue)
{
    net::Socket s1(net::Protocol::Tcp);
    EXPECT_EQ(s1.isOpen(), true);

    net::Socket s2;
    s2.setHandle(s1.getHandle());
    EXPECT_EQ(s2.isOpen(), true);
    EXPECT_EQ(s1 == std::move(s2), true);
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
