// Web server connection example

#include <net/netcpp.hpp>
#include <iostream>
#include <future>
#include "Exception.hpp"

int main() {
    try {
        net::Socket sock(net::Protocol::Tcp);
        if (!sock.isOpen())
            return -1;

        auto context = new net::Context();
        context->endpoint = std::make_unique<net::Endpoint>(net::IpAddress::Loopback, 8081);
        context->completed = [](net::Context* context, bool isSuccess) {
            if (isSuccess)
                std::cout << "Connected!\n";
            else
                std::cout << "Failed\n";
            exit(0);
        };
        if (!sock.connect(context))
            throw net::network_error("conn");
        getchar();
        delete context;
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}