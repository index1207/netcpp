#pragma once

#include <functional>

#include "Socket.hpp"

namespace net
{
	enum class SocketError
	{
		Error = -1,
		Success = 1,
	};

	enum class EventType
	{
		Accept,
		Connect,
		Disconnect,
		Send,
		Recv
	};

	class SocketAsyncEvent
		: public OVERLAPPED, public std::enable_shared_from_this<AcceptEvent>
	{
		void Init();
	public:
		SocketAsyncEvent(EventType eventType);
		EventType eventType;
		SocketError socketError;
		std::function<void(SocketAsyncEvent*)> completed;
	};

	class AcceptEvent : public SocketAsyncEvent
	{
	public:
		AcceptEvent() : SocketAsyncEvent(EventType::Accept)
		{
			ZeroMemory(_acceptexBuffer, sizeof(_acceptexBuffer));
		}
		std::shared_ptr<Socket> acceptSocket;
		char _acceptexBuffer[(sizeof(SOCKADDR_IN) + 16) * 2];
	};

	class ConnectEvent : public SocketAsyncEvent
	{
	public:
		ConnectEvent() : SocketAsyncEvent(EventType::Connect) { }
		IPEndPoint endPoint {};
	};

	class SendEvent : public SocketAsyncEvent
	{
	public:
		SendEvent() : SocketAsyncEvent(EventType::Send) { }
		ArraySegment segment {};
		int sentBytes = 0;
	};

	class RecvEvent : public SocketAsyncEvent
	{
	public:
		RecvEvent() : SocketAsyncEvent(EventType::Recv) { }
		ArraySegment segment {};
		int recvBytes = 0;
	};

	class DisconnectEvent : public SocketAsyncEvent
	{
	public:
		DisconnectEvent() : SocketAsyncEvent(EventType::Disconnect) { }
	};
}

