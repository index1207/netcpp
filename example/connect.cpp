// Web server connection example

#include <net/netcpp.hpp>
#include <iostream>
#include <future>

int main() {
    net::Socket sock(net::Protocol::Tcp);
    if (!sock.isOpen())
        return -1;

    auto context = new net::Context();
    context->endpoint = std::make_unique<net::Endpoint>(net::IpAddress::Loopback, 8081);
    context->completed = [](net::Context *context) {
        if (context->isSuccess)
            std::cout << "Connected!\n";
        else
            std::cout << "Failed\n";
        exit(0);
    };
    if (!sock.connect(context))
        return -1;

    getchar();

    return 0;
}