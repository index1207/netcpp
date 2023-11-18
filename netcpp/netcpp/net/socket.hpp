#pragma once

#include <winsock2.h>
#include "endpoint.hpp"

namespace net
{
	enum class address_family
	{
		IPV4 = AF_INET,
		IPV6 = AF_INET6
	};

	enum class socket_type
	{
		STREAM = SOCK_STREAM,
		DGRAM = SOCK_DGRAM
	};

	enum class protocol
	{
		TCP = IPPROTO_TCP,
		UDP = IPPROTO_UDP
	};

	enum class option_level
	{
		IP = SOL_IP,
		IPV6 = SOL_IPV6,
		SOCKET = SOL_SOCKET,
	};

	enum class option_name
	{
		// socket Level
		LINGER = SO_LINGER,
		REUSE_ADDRESS = SO_REUSEADDR,
		SEND_BUFFER = SO_SNDBUF,
		RECV_BUFFER = SO_RCVBUF,
		BROADCAST = SO_BROADCAST,

		// IP Level
		TTL = 4,
		
		// TCP Level
		NO_DELAY = TCP_NODELAY,
	};

	struct linger
	{
		bool enabled;
		int time;
	};

	struct ArraySegment
	{
		ArraySegment() = default;
		explicit ArraySegment(const char* array, int offset, int count)
		{
			Array = const_cast<char*>(array);
			Offset = offset;
			Count = count;
		}
		char* Array;
		int Offset;
		int Count;
	};

	namespace context
	{
		class accept;
		class connect;
		class send;
		class receive;
		class disconnect;
	}
	
	class socket
	{
	public:
		socket();
		socket(address_family af, socket_type st, protocol pt);
		socket(address_family af, socket_type st);
		socket(const socket& sock);
		socket(socket&& sock) noexcept;
		~socket();
	public:
		void close();

		void set_handle(SOCKET s);
		
		bool bind(endpoint ep);
		bool listen(int backlog = SOMAXCONN) const;
	public:
		SOCKET get_handle() const;
		endpoint get_remote() const;
		endpoint get_local() const;
	public:
		void set_remote(endpoint ep);
		void set_local(endpoint ep);
	public:
		void disconnect();
		bool disconnect_async(context::disconnect* disconnectEvent) const;

		socket accept() const;
		bool accept_async(context::accept* acceptEvent) const;

		bool connect(endpoint ep);
		bool connect_async(context::connect* connectEvent);

		int send(ArraySegment seg) const;
		int send(ArraySegment seg, endpoint target) const;
		bool send_async(context::send* sendEvent) const;

		int receive(ArraySegment seg) const;
		int receive(ArraySegment seg, endpoint target) const;
		bool receive_async(context::receive* recvEvent) const;
	public:
		template<class T>
		void set_socket_option(option_level level, option_name name, T value) const
		{
			setsockopt(_sock,
				static_cast<int>(level),
				static_cast<int>(name),
				reinterpret_cast<const char*>(&value),
				sizeof(T)
			);
		}
		
		void set_blocking(bool isBlocking) const;
		void set_linger(linger linger) const;
		void set_broadcast(bool isBroadcast) const;
		void set_reuse_address(bool isReuseAddr) const;
		void set_no_delay(bool isNoDelay) const;
		void set_ttl(int ttl) const;
		void set_send_buffer(int size) const;
		void set_receive_buffer(int size) const;
		bool valid() const;
	public:
		socket& operator=(const socket& sock);
		socket& operator=(socket&& sock) noexcept;
	private:
		endpoint _remoteEp;
		endpoint _localEp;
		SOCKET _sock;
	};
}