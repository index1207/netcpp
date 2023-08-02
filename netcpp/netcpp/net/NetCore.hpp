#pragma once

unsigned CALLBACK Worker(HANDLE hcp);

class NetCore
{
public:
	NetCore();
	~NetCore();
public:
	void Register(class Socket* s);
private:
	HANDLE _hcp;
	std::thread _worker;
};

extern NetCore GNetCore;