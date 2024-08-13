# netcpp ![windows](https://github.com/index1207/netcpp/actions/workflows/windows.yml/badge.svg) ![linux](https://github.com/index1207/netcpp/actions/workflows/linux.yml/badge.svg) [![codecov](https://codecov.io/gh/index1207/netcpp/graph/badge.svg?token=BVVUC5S422)](https://codecov.io/gh/index1207/netcpp) ![lang](https://img.shields.io/badge/language-C++20-blue) [![Vcpkg package](https://img.shields.io/badge/vcpkg-netcpp-blue)](https://github.com/microsoft/vcpkg/tree/master/ports/netcpp) [![License](https://img.shields.io/github/license/index1207/netcpp.svg)](LICENSE)
netcpp is open-source simple C++ network library. netcpp supports windows and linux(ubuntu) platform. asynchronous feature implement by each os's api. Windows implemented using IOCP and Ubuntu will implement using Epoll.

## Installation
To use netcpp, create new application by vcpkg or enable manifest mode at Visual Studio. <br>
At commend line:
```shell
vcpkg install netcpp
```
At vcpkg.json:
```json
{
  "dependencies": [
    "netcpp"
  ]
}
```
Or clone this repo and build.
```shell
git clone https://github.com/index1207/netcpp.git && cd netcpp
cmake -B build
cmake --build build
```

## Example and Features
- Basic connection
```cpp
// Server
#include <net/Socket.hpp>
#include <iostream>
      
int main()
{
    net::Native::initialize(); // Initialize Native API
  
    net::Socket sock(net::protocol::tcp); // Create new TCP socket
    if (!sock.is_open()) // Invalidate socket
        return -1;
    if(!sock.bind(net::Endpoint(net::IpAddress::loopback, 8085))) // Bind address
        return -1;
    if(!sock.listen()) // Ready to accept
        return -1;
          
    while(true)
    {
        auto client = sock.accept(); // Accept other client. it returns new client socket.
        std::cout << "Connected\n";
    }
}
```
```cpp
// Client
#include <net/Socket.hpp>
#include <iostream>
  
int main()
{
    net::Native::initialize(); // Initialize Native API
  
    net::Socket sock(net::protocol::tcp); // Create new TCP socket
    if (!sock.is_open()) // Invalidate socket
        return -1;
    if (!sock.connect(net::Endpoint(net::IpAddress::loopback, 8085))) // Try to connect to server.
        return -1;
    std::cout << "Connected!";
}
```
## Minimum required compiler version
- Windows
  - Visual Studio 2019
- Linux
  - Clang 16
  - GCC 13.2