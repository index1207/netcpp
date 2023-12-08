// Web server connection example

#include <net/netcpp.hpp>
#include <iostream>
#include <future>

int main() {
   net::Socket listenSock(net::Protocol::Tcp);
   if(!listenSock.isOpen())
       return -1;

   auto context = new net::Context();
   context->endpoint = std::make_unique<net::Endpoint>(net::IpAddress::Loopback, 8080);
   context->completed = [](net::Context* context) {
       if(context->isSuccess)
           std::cout << "Connected!\n";
       else
           std::cout << "Failed\n";
       exit(0);
   };

   if(!listenSock.connect(context))
       return -1;
   getchar();

   return 0;
}