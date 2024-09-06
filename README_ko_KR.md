# netcpp ![windows](https://github.com/index1207/netcpp/actions/workflows/windows.yml/badge.svg) ![linux](https://github.com/index1207/netcpp/actions/workflows/linux.yml/badge.svg) [![codecov](https://codecov.io/gh/index1207/netcpp/graph/badge.svg?_token=BVVUC5S422)](https://codecov.io/gh/index1207/netcpp) ![lang](https://img.shields.io/badge/language-C++20-blue) [![Vcpkg package](https://img.shields.io/badge/vcpkg-0.4.1-yellow)](https://github.com/microsoft/vcpkg/tree/master/ports/netcpp) [![License](https://img.shields.io/github/license/index1207/netcpp.svg)](LICENSE)
[[돌아가기]](https://github.com/index1207/netcpp) <br>
netcpp는 간단하게 사용할 수 있는 크로스플랫폼 C++ 네트워크 라이브러리로, Windows와 Linux 플랫폼을 지원합니다.

## 설치
이 라이브러리는 [vcpkg](https://github.com/microsoft/vcpkg)포트를 지원합니다. 만약 이미 vcpkg가 설치되어 있다면 아래의 명령줄을 통해 설치가 가능합니다.
```shell
vcpkg install netcpp  # 클래식 모드에서
vcpkg add port netcpp # 매니페스트 모드에서
```

직접 클론하고 빌드하여 사용할 수 있습니다.
```shell
git clone https://github.com/index1207/netcpp.git && cd netcpp # 클론 및 디렉토리 이동
cmake -B build                                                 # CMake 설정
cmake --build build --config Debug                             # 디버그 모드로 빌드
cmake --build build --config Release                           # 릴리즈 모드로 빌드
cmake --install build --prefix {PATH_TO_INSTALL}               # 다른 프로젝트에 설치
```

netcpp는 다음의 CMake 타겟을 제공합니다.
```cmake
find_package(netcpp CONFIG REQUIRED)
target_link_libraries(main PRIVATE netcpp::netcpp)
```

## CMake 옵션
| 옵션                   | 설명                |
|-----------------------|-------------------|
| `NETCPP_BUILD_SHARED` | 공유 라이브러리로 빌드합니다.  |
| `NETCPP_TEST`         | 유닛테스트를 빌드에 포함합니다. |

## 매크로
| 매크로             | 설명                                    |
|-----------------|---------------------------------------|
| `NETCPP_STATIC` | 정적 라이브러리로 사용할 시, 반드시 이 매크로를 정의해야 합니다. |


## 예제
- 소켓 생성
```cpp
// <net/socket.hpp>
net::socket tcp_socket(net::protocol::tcp); // 새 TCP 소켓 생성

net::socket udp_socket(net::protocol::udp); // 새 UDP 소켓 생성

net::socket empty_socket;
empty_socket.create(net::protocol::tcp); // 메서드를 통한 새 TCP 소켓 생성
```
- 비동기 I/O
```cpp
// <net/context.hpp>
net::socket sock(net::protocol::tcp); // 새 TCP 소켓 생성
net::context connect_ctx; // 비동기 I/O에 필요한 컨텍스트 변수
connect_ctx.endpoint = ENDPOINT; // 연결 엔드포인트 지정
connect_ctx.completed = [](net::context* ctx, bool success) { // 콜백
	if (success)
	{
		std::cout << "Connected" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect" << std::endl;
	}
};
sock.connect(&connect_ctx); // 지정된 엔드포인트에 연결 시도
```
- 기본 연결
```cpp
// Server
#include <net/Socket.hpp>
#include <iostream>
      
int main()
{
    net::native::initialize(); // 네이티브 API 초기화
  
    net::socket sock(net::protocol::tcp); // 새 TCP 소켓 생성
    if (!sock.is_open()) // 소켓이 유효한지 검사
        return -1;
    if (!sock.bind(net::endpoint(net::ip_address::loopback, 8085))) // `tcp://loopback:8085`로 주소 할당
        return -1;
    if (!sock.listen()) // accept의 준비
        return -1;
          
    while (true)
    {
        auto client = sock.accept(); // 다른 클라이언트 접속. 접속된 소켓 리턴
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
    net::native::initialize(); // 네이티브 API 초기화
  
    net::socket sock(net::protocol::tcp); // 새 TCP 소켓 생성
    if (!sock.is_open()) // 소켓이 유효한지 검사
        return -1;
    if (!sock.connect(net::endpoint(net::ip_address::loopback, 8085))) // 서버에 연결 시도
        return -1;
    std::cout << "Connected!";
}
```
- DNS
```cpp
// <net/dns.hpp>
net::dns::get_host_name() // 호스트 이름 리턴

net::dns::get_host_entry("www.example.com") // www.example.com의 호스트 엔트리 리턴
```
- 예외처리
```cpp
try {
	if (!sock.connect(ENDPOINT))
	    throw net::network_exception("connect()");
}
catch(std::exception& e) {
	std::cout << e.what() << std::endl; // connect(): 요청된 주소를 할당할 수 없습니다. [10049]
}
```

## 종속성
| OS      | Library  |
|---------|----------|
| Windows | Winsock2 |
| Linux   | liburing |

## 기여
이 레포지토리는 언제나 이슈나 PR을 환영합니다!

## 최소 컴파일러 버전
- Windows
    - Visual Studio 2019
- Linux
    - Clang 12
    - GCC 10