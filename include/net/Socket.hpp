#pragma once

#include <span>

#include "IpAddress.hpp"
#include "Endpoint.hpp"

namespace net
{
	enum class AddressFamily
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6
	};

	enum class SocketType
	{
		Stream = SOCK_STREAM,
		Dgram = SOCK_DGRAM
	};

	enum class Protocol
	{
		Tcp = IPPROTO_TCP,
		Udp = IPPROTO_UDP
	};

	enum class OptionLevel
	{
		IP = SOL_IP,
		IPv6 = SOL_IPV6,
		Socket = SOL_SOCKET,
	};

	enum class OptionName
	{
		// Socket Level
		Linger = SO_LINGER,
		ReuseAddress = SO_REUSEADDR,
		SendBuffer = SO_SNDBUF,
		RecvBuffer = SO_RCVBUF,
		Broadcast = SO_BROADCAST,

		// IP Level
		TTL = 4,
		
		// Tcp Level
		NoDelay = TCP_NODELAY,
	};

	struct Linger
	{
		bool enabled;
		int time;
	};

	class Socket
    {
	public:
		Socket();
		Socket(AddressFamily af, SocketType st, Protocol pt);
		Socket(AddressFamily af, SocketType st);
		Socket(const Socket& sock);
		Socket(Socket&& sock) noexcept;
		~Socket();
	public:
		void close();

		void setHandle(SOCKET s);
		
		bool bind(Endpoint ep);
		bool listen(int backlog = SOMAXCONN) const;
	public:
		SOCKET getHandle() const;
		Endpoint getRemoteEndpoint() const;
		Endpoint getLocalEndpoint() const;
	public:
		void setRemoteEndpoint(Endpoint ep);
		void setLocalEndpoint(Endpoint ep);
	public:
		void disconnect();
		bool disconnect(class DisconnectContext* disconnectEvent) const;

		Socket accept() const;
		bool accept(class AcceptContext* event) const;

		bool connect(Endpoint ep);
		bool connect(class ConnectContext* event);

		int send(std::span<char> s) const;
		int send(std::span<char> s, Endpoint target) const;
		bool send(class SendContext* sendEvent) const;

		int receive(std::span<char> s) const;
		int receive(std::span<char> s, Endpoint target) const;
		bool receive(class ReceiveContext* recvEvent) const;
	public:
		template<class T>
		void setSocketOption(OptionLevel level, OptionName name, T value) const
		{
			setsockopt(_sock,
				static_cast<int>(level),
				static_cast<int>(name),
				reinterpret_cast<const char*>(&value),
				sizeof(T)
			);
		}
		
		void setBlocking(bool isBlocking) const;
		void setLinger(Linger linger) const;
		void setBroadcast(bool isBroadcast) const;
		void setReuseAddress(bool isReuseAddr) const;
		void setNoDelay(bool isNoDelay) const;
		void setTTL(int ttl) const;
		void setSendBuffer(int size) const;
		void setReceiveBuffer(int size) const;
		bool isOpen() const;
	public:
		Socket& operator=(const Socket& sock);
		Socket& operator=(Socket&& sock) noexcept;
	private:
		Endpoint _remoteEp;
		Endpoint _localEp;
		SOCKET _sock;
	};
}