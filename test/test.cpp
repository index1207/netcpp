#include <iostream>
#include <net/netcpp.hpp>

using namespace std;
using namespace net;

Socket sock;

void static OnCompleted(Context* context) {
    cout << "Connected!\n";
    sock.accept(context);
}

int main() {
    sock.create(Protocol::Tcp);

    sock.bind(Endpoint(IpAddress::Loopback, 9999));
    sock.listen();

    auto context = new Context;
    context->completed = OnCompleted;
    sock.accept(context);

    getchar();
}