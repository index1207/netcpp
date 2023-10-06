#pragma once

#include "IPEndPoint.hpp"

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
		~Socket();
	public:
		void SetHandle(SOCKET s);

		void Close();

		bool Bind(IPEndPoint ep);
		bool Listen(int backlog = SOMAXCONN);
	public:
		SOCKET GetHandle() const;
		IPEndPoint GetRemoteEndPoint() const;
		IPEndPoint GetLocalEndPoint() const;
	public:
		void SetRemoteEndPoint(IPEndPoint ep);
		void SetLocalEndPoint(IPEndPoint ep);
	public:
		void Disconnect();
		bool DisconnectAsync(std::shared_ptr<class DisconnectEvent> disconnectEvent);

		Socket Accept();
		bool AcceptAsync(std::shared_ptr<class AcceptEvent> acceptEvent);

		bool Connect(IPEndPoint ep);
		bool ConnectAsync(std::shared_ptr<class ConnectEvent> connectEvent);

		int Send(ArraySegment seg);
		bool SendAsync(std::shared_ptr<class SendEvent> sendEvent);

		int Receive(ArraySegment seg);
		bool ReceiveAsync(std::shared_ptr<class RecvEvent> recvEvent);
	public:
		void SetBlocking(bool isBlocking);
		bool IsValid() const;
	public:
		void operator=(Socket sock);
	private:
		std::unique_ptr<IPEndPoint> _remoteEp;
		std::unique_ptr<IPEndPoint> _localEp;
		SOCKET _sock;
	};
}