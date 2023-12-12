#include <iostream>
#include <net/netcpp.hpp>

using namespace std;
using namespace net;


int main() {
    Socket sock(Protocol::Tcp);

    sock.bind(Endpoint(IpAddress::Loopback, 9999));
    sock.listen();

    Context context;
    context.completed = [&sock](Context* context) {
        cout << "Connected\n";
        sock.accept(context);
    };
    sock.accept(&context);

    getchar();
}