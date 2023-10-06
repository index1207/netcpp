#pragma once

#include "IPEndPoint.hpp"
#include <memory>

namespace net
{
	enum class AddressFamily
	{
		Internetwork = AF_INET,
		Internetwork6 = AF_INET6
	};

	enum class SocketType
	{
		Stream = SOCK_STREAM,
		Dgram = SOCK_DGRAM
	};

	enum class ProtocolType
	{
		Tcp = IPPROTO_TCP,
		Udp = IPPROTO_UDP
	};

	enum class SocketOptionLevel
	{
		IP = SOL_IP,
		IPv6 = SOL_IPV6,
		Socket = SOL_SOCKET,
	};

	enum class SocketOptionName
	{
		// Socket Level
		Linger = SO_LINGER,
		ReuseAddress = SO_REUSEADDR,
		SendBuffer = SO_SNDBUF,
		ReceiveBuffer = SO_RCVBUF,
		Broadcast = SO_BROADCAST,

		// IP Level
		TTL = IP_TTL,
		
		// TCP Level
		NoDelay = TCP_NODELAY,
	};

	struct LingerOption
	{
		bool Enabled;
		int LingerTime;
	};

	struct ArraySegment
	{
		ArraySegment() = default;
		explicit ArraySegment(char* array, int offset, int count)
		{
			Array = array;
			Offset = offset;
			Count = count;
		}
		char* Array;
		int Offset;
		int Count;
	};

	class Socket
	{
	public:
		Socket();
		Socket(AddressFamily af, SocketType st, ProtocolType pt);
		Socket(AddressFamily af, SocketType st);
		Socket(const Socket& sock);
		Socket(Socket&& sock) noexcept;
		~Socket();
	public:
		void Close();

		void SetHandle(SOCKET s);
		
		bool Bind(IPEndPoint ep);
		bool Listen(int backlog = SOMAXCONN) const;
	public:
		SOCKET GetHandle() const;
		IPEndPoint GetRemoteEndPoint() const;
		IPEndPoint GetLocalEndPoint() const;
	public:
		void SetRemoteEndPoint(IPEndPoint ep);
		void SetLocalEndPoint(IPEndPoint ep);
	public:
		void Disconnect();
		bool DisconnectAsync(const std::shared_ptr<class DisconnectEvent>& disconnectEvent) const;

		Socket Accept() const;
		bool AcceptAsync(std::shared_ptr<class AcceptEvent> acceptEvent) const;

		bool Connect(IPEndPoint ep);
		bool ConnectAsync(std::shared_ptr<class ConnectEvent> connectEvent);

		int Send(ArraySegment seg) const;
		int SendTo(ArraySegment seg, IPEndPoint target) const;
		bool SendAsync(std::shared_ptr<class SendEvent> sendEvent) const;

		int Receive(ArraySegment seg) const;
		int ReceiveFrom(ArraySegment seg, IPEndPoint target) const;
		bool ReceiveAsync(std::shared_ptr<class RecvEvent> recvEvent) const;
	public:
		template<class T>
		void SetSocketOption(SocketOptionLevel level, SocketOptionName name, T value) const
		{
			setsockopt(_sock,
				static_cast<int>(level),
				static_cast<int>(name),
				reinterpret_cast<const char*>(&value),
				sizeof(T)
			);
		}
		
		void SetBlocking(bool isBlocking) const;
		void SetLinger(LingerOption linger) const;
		void SetBroadcast(bool isBroadcast) const;
		void SetReuseAddress(bool isReuseAddr) const;
		void SetNoDelay(bool isNoDelay) const;
		void SetTTL(int ttl) const;
		void SetSendBufferSize(int size) const;
		void SetReceiveBufferSize(int size) const;
		bool IsValid() const;
	public:
		Socket& operator=(const Socket& sock);
		Socket& operator=(Socket&& sock) noexcept;
	private:
		std::unique_ptr<IPEndPoint> _remoteEp;
		std::unique_ptr<IPEndPoint> _localEp;
		SOCKET _sock;
	};
}