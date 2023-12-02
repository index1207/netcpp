# netcpp
<image src="https://github.com/index1207/netcpp/assets/63224377/5adcc63a-50e2-42a7-bcd5-d568ff1500a9" width="35%"> <br>
[![Windows Build](https://github.com/index1207/netcpp/actions/workflows/cmake-windows-platform.yml/badge.svg?branch=release)](https://github.com/index1207/netcpp/actions/workflows/cmake-windows-platform.yml) ![](https://img.shields.io/badge/language-C++20-blue)<br>
`netcpp` is so simple c++ Netowrk Library. <br>
this library is based on Windows Single-Platform based CMake.

## Build
To build this library, you need upper VS2019 compiler version.
1. Generate Visual Studio project. <br>
`cmake .`
2. Open .sln project and build.

## Sample Code

### Client
```cpp
#include <net/netcpp.hpp>
#include <iostream>

int main() {
    net::Socket sock(net::Protocol::Tcp);
    if (!sock.isOpen()) {
        return -1;
    }
    if (!sock.connect(net::Endpoint(net::IpAddress::Loopback, 8085))) {
        return -1;
    }
    std::cout << "Connected!";
}
```

### Server
```cpp
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
```
