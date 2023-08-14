#pragma once

namespace net
{
	class Agent
	{
	public:
		Agent(class Socket* s);
		virtual ~Agent();
	public:
		virtual void OnSend(int len) { };
		virtual int OnRecv(char* buffer, int len) { };
		virtual void OnConnected() { };
		virtual void OnDisconnected() { };
	public:
		Socket GetSocket();
	private:
		Socket* _sock;
	};
}