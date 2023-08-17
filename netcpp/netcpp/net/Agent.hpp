#pragma once

#include "Socket.hpp"
#include "SocketAsyncEvent.hpp"

namespace net
{
	class Agent abstract
	{
	public:
		Agent() = default;
		virtual ~Agent();
	public:
		virtual void OnSend(int len) abstract;
		virtual int OnRecv(char* buffer, int len) abstract;
		virtual void OnConnected() abstract;
		virtual void OnDisconnected() abstract;
	public:
		void Run(std::shared_ptr<Socket> s);
	private:
		void Disconnect();
		void PostRecv(RecvEvent* recvEvent);
	public:
		Socket GetSocket();
	private:
		std::shared_ptr<Socket> _sock;
		RecvEvent _recvEvent;
		std::atomic_bool _isDisconnect;
	};
}