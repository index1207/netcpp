#include <net/netcpp.hpp>
#include <net/Exception.hpp>
#include <iostream>

int main() {
    net::Socket sock(net::Protocol::Tcp);
    try {
        if (!sock.isOpen())
            throw net::network_error("isOpen()");
        if (!sock.bind(net::Endpoint(net::IpAddress::Loopback, 8085)))
            throw net::network_error("bind()");
        if (!sock.listen())
            throw net::network_error("listen()");

        net::Context context;
        context.completed = [sock](net::Context* context, bool success) {
            std::cout << "CONNECTED\n";
            sock.accept(context);
        };
        if (!sock.accept(&context))
            throw net::network_error("accept()");
        getchar();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}