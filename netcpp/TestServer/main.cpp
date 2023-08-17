// Server
#include <iostream>
#include <thread>
#include <net/netcpp.hpp>

using namespace net;

class MyAgent : public Agent
{
public:
	MyAgent() = default;
public:
	virtual void OnSend(int len) override
	{

	}
	virtual int OnRecv(char* buffer, int len) override
	{
		std::cout << "OnRecv : " << buffer << ", " << len << '\n';
		return len;
	}
	virtual void OnConnected() override
	{
		std::cout << "Connected!!!!\n";
	}
	virtual void OnDisconnected() override
	{
		std::cout << "Disconnected\n";
	}
};

int main()
{
	auto listener = new Listener(IPEndPoint(IPAddress::Any, 8080), []()
		{
			return new MyAgent;
		});

	if (!listener->Run())
		return -1;

	while (true)
	{
		Sleep(100);
	}

	return 0;
}