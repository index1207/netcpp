// Web server connection example

#include <iostream>
#include <future>

int main() {
    try {
        net::Socket sock(net::Protocol::Tcp);
        if (!sock.isOpen())
            return -1;

        auto context = new net::Context();
        context->endpoint = net::Endpoint(net::IpAddress::Loopback, 1225);
        context->completed = [&](net::Context* context, bool isSuccess) {
            std::string v = "hello";

            auto ctx = new net::Context;
            ctx->buffer = v;
            sock.send(ctx);
        };
        if (!sock.connect(context))
            throw net::network_error("conn");
        getchar();
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}