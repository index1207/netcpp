// Web server connection example

#include <net/netcpp.hpp>
#include <iostream>

int main() {
   net::Socket listenSock(net::AddressFamily::IPv4, net::SocketType::Stream);
   if(!listenSock.isOpen())
       return -1;
   if(!listenSock.connect(net::Endpoint(net::IpAddress::Loopback, 8080)))
       return -1;
   std::cout << "Connected!\n";
}