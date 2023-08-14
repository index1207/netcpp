#pragma once

namespace net
{
	unsigned CALLBACK Worker(HANDLE hcp);

	class NetCore
	{
	public:
		NetCore();
		~NetCore();
	public:
		void Register(class Socket* s);
		void Register(SOCKET s);
		void Register(class Agent* agent);
	private:
		HANDLE _hcp;
		std::thread _worker;
	};

	extern NetCore GNetCore;
}