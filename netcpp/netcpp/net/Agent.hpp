#pragma once

#include "Socket.hpp"
#include "SocketAsyncEvent.hpp"

namespace net
{
	class Agent abstract
	{
	public:
		Agent() = default;
		Agent(Socket s);
		virtual ~Agent();
	public:
		virtual void OnSend(int len) abstract;
		virtual int OnRecv(char* buffer, int len) abstract;
		virtual void OnConnected() abstract;
		virtual void OnDisconnected() abstract;
	public:
		void Run(Socket s);
	private:
		void Disconnect();
		void PostRecv(RecvEvent* recvEvent);
	public:
		Socket GetSocket();
	private:
		Socket _sock;
		RecvEvent _recvEvent;
	};
}