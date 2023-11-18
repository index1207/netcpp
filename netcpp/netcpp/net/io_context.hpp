#pragma once

#include <functional>
#include <iostream>

#include "socket.hpp"

namespace net
{
	enum class socket_error
	{
#undef ERROR
		ERROR = -1,
		SUCCESS = 1,
	};

	enum class context_type
	{
		ACCEPT,
		CONNECT,
		DISCONNECT,
		SEND,
		RECEIVE
	};

	class io_context
		: public OVERLAPPED
	{
		void Init();
	public:
		io_context(context_type eventType);
		context_type eventType;
		socket_error socketError;
		std::function<void(io_context*)> completed;
	};

	namespace context
	{
		class accept : public io_context
		{
		public:
			accept() : io_context(context_type::ACCEPT), acceptSocket(nullptr)
			{
				ZeroMemory(_acceptexBuffer, sizeof(_acceptexBuffer));
			}
			std::unique_ptr<socket> acceptSocket;
			char _acceptexBuffer[(sizeof(SOCKADDR_IN) + 16) * 2];
		};

		class connect : public io_context
		{
		public:
			connect() : io_context(context_type::CONNECT) { }
			endpoint endPoint {};
		};

		class send : public io_context
		{
		public:
			send() : io_context(context_type::SEND) { }
			ArraySegment segment {};
			int sentBytes = 0;
		};

		class receive : public io_context
		{
		public:
			receive() : io_context(context_type::RECEIVE) { }
			ArraySegment segment {};
			int recvBytes = 0;
		};

		class disconnect : public io_context
		{
		public:
			disconnect() : io_context(context_type::DISCONNECT) { }
		};
	}
}

