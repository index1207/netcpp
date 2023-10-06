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
		void SetHandle(SOCKET s);

		void Close();

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
		bool AcceptAsync(const std::shared_ptr<class AcceptEvent>& acceptEvent) const;

		bool Connect(IPEndPoint ep);
		bool ConnectAsync(const std::shared_ptr<class ConnectEvent>& connectEvent);

		int Send(ArraySegment seg) const;
		bool SendAsync(const std::shared_ptr<class SendEvent>& sendEvent) const;

		int Receive(ArraySegment seg) const;
		bool ReceiveAsync(const std::shared_ptr<class RecvEvent>& recvEvent) const;
	public:
		void SetBlocking(bool isBlocking) const;
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