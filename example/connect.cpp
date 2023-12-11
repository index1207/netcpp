// Web server connection example

#include <net/netcpp.hpp>
#include <iostream>
#include <future>

int main() {
    try {
        net::Socket sock(net::Protocol::Tcp);
        if (!sock.isOpen())
            return -1;

        auto context = new net::Context();
        auto ep = net::Endpoint(net::IpAddress::Loopback, 8080);
        context->endpoint = &ep;
        context->completed = [](net::Context* context) {
            if (context->isSuccess)
                std::cout << "Connected!\n";
            else
                std::cout << "Failed\n";
            exit(0);
        };
        if (!sock.connect(context))
            return -1;
        getchar();
        delete context;
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}