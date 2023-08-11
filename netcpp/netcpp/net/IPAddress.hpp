#pragma once

class IPAddress : private SOCKADDR_IN
{
	friend class IPEndPoint;
public:
	static IPAddress Any;
	static IPAddress None;
	static IPAddress Loopback;
	static IPAddress Broadcast;
public:
	static IPAddress Parse(std::string_view ipStr);
	static IPAddress Parse(int ipNum);
public:
	IPAddress();
	IPAddress(const SOCKADDR_IN& adrs);
public:
	std::string ToString() const;
};

