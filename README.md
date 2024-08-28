# netcpp ![windows](https://github.com/index1207/netcpp/actions/workflows/windows.yml/badge.svg) ![linux](https://github.com/index1207/netcpp/actions/workflows/linux.yml/badge.svg) [![codecov](https://codecov.io/gh/index1207/netcpp/graph/badge.svg?_token=BVVUC5S422)](https://codecov.io/gh/index1207/netcpp) ![lang](https://img.shields.io/badge/language-C++20-blue) [![Vcpkg package](https://img.shields.io/badge/vcpkg-0.4.1-yellow)](https://github.com/microsoft/vcpkg/tree/master/ports/netcpp) [![License](https://img.shields.io/github/license/index1207/netcpp.svg)](LICENSE)
netcpp is open-source **simple** C++ network library.
It supports windows and linux platform.

## Installation
This library supports [vcpkg](https://github.com/microsoft/vcpkg) port. If you had already installed vcpkg, You can install this package simply with the command line below.
```shell
vcpkg install netcpp
```
In manifest mode, you can add this package in `vcpkg.json`.
```json
{
  "dependencies": [
    "netcpp"
  ]
}
```

Or clone this repository, and execute the command line below.
```shell
git clone https://github.com/index1207/netcpp.git && cd netcpp
cmake -B build
cmake --build build --config Debug
cmake --build build --config Release
cmake --install build --prefix {PATH_TO_INSTALL}
```

The usage
```text
netcpp provides CMake targets:

    find_package(netcpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE netcpp::netcpp)
```

## Example
- Create a socket
```cpp
// <net/socket.hpp>
net::socket tcp_socket(net::protocol::tcp); // Create a TCP socket

net::socket udp_socket(net::protocol::udp); // Create a UDP socket

net::socket empty_socket;
empty_socket.create(net::protocol::tcp); // Create a new TCP socket
```
- Async I/O
```cpp
// <net/context.hpp>
net::socket sock(net::protocol::tcp); // Create a new TCP socket.
net::context connect_ctx;
connect_ctx.endpoint = ENDPOINT; // Specify the endpoint.
connect_ctx.completed = [](net::context* ctx, bool success) { // callback
	if (success)
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect" << std::endl;
	}
};
sock.connect(&connect_ctx); // Connect to specified endpoint asynchronously.
```
- Basic connection
```cpp
// Server
#include <net/Socket.hpp>
#include <iostream>
      
int main()
{
    net::native::initialize(); // Initialize Native API
  
    net::socket sock(net::protocol::tcp); // Create new TCP socket
    if (!sock.is_open()) // Validate socket
        return -1;
    if (!sock.bind(net::endpoint(net::ip_address::loopback, 8085))) // Bind the address `tcp://loopback:8085`
        return -1;
    if (!sock.listen()) // Ready to accept
        return -1;
          
    while(true)
    {
        auto client = sock.accept(); // accept other client. it returns new client socket.
        std::cout << "Connected\n";
    }
}
```
```cpp
// Client
#include <net/socket.hpp>
#include <iostream>
  
int main()
{
    net::native::initialize(); // Initialize Native API
  
    net::socket sock(net::protocol::tcp); // Create new TCP socket
    if (!sock.is_open()) // Validate socket
        return -1;
    if (!sock.connect(net::endpoint(net::ip_address::loopback, 8085))) // Try to connect to server.
        return -1;
    std::cout << "Connected!";
}
```
- DNS
```cpp
// <net/dns.hpp>
net::dns::get_host_name() // get host's name

net::dns::get_host_entry("www.example.com") // get www.example.com's host entry
```
- Exception
```cpp
try {
	if (!sock.connect(ENDPOINT))
	    throw net::network_exception("connect()");
}
catch(std::exception& e) {
	std::cout << e.what() << std::endl; // connect(): Cannot assign requested address. [10049]
}
```

## Dependencies
| OS      | Library    |
|---------|------------|
| Windows | ws2_32.lib |
| Linux   | liburing.a |

## Contribute
The repository is whenever welcome any issues or PRs!

## Minimum required compiler version
- Windows
  - Visual Studio 2019
- Linux
  - Clang 10
  - GCC 10