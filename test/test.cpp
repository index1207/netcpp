#include <net/netcpp.hpp>
#include <iostream>

int main() {
    net::Socket sock(net::Protocol::Tcp);
    if (!sock.isOpen()) {
        return -1;
    }
    if(!sock.bind(net::Endpoint(net::IpAddress::Loopback, 8085))) {
        return -1;
    }
    if(!sock.listen()) {
        return -1;
    }

    while(true) {
        auto client = sock.accept();
        std::cout << "Connected\n";
    }
}