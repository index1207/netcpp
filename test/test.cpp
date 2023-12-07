#include <iostream>
#include <net/netcpp.hpp>

using namespace std;
using namespace net;

Socket sock;
Context ov;

void static OnCompleted(Context* context) {
    cout << "Connected!\n";
    sock.accept(context);
}

int main() {
    sock.create(Protocol::Tcp);

    sock.bind(Endpoint(IpAddress::Loopback, 9999));
    sock.listen();

    ov.completed = OnCompleted;
    sock.accept(&ov);

    getchar();
}