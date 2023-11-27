#include <iostream>
#include <vector>

#include <net/netcpp.hpp>

using namespace std;
using namespace net;

int main() {
    Socket sock(AddressFamily::IPv4, net::SocketType::Stream);
    if(!sock.isOpen())
        return -1;
    sock.bind(Endpoint(IpAddress::Loopback, 9999));
    sock.listen();

    string data = "HEllo";
    while (true) {
        auto s = sock.accept();
        s.send(data);
    }
}