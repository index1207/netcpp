#include "net/native.hpp"
#include "net/socket.hpp"
#include "net/context.hpp"

#include <random>
#include <thread>

using namespace net;

bool native::option::auto_run = true;
unsigned native::option::thread_count = std::thread::hardware_concurrency();
#ifdef _WIN32
unsigned long native::option::timeout = INFINITE;
#else
unsigned long native::option::timeout = 0;
#endif
u_int native::option::entry_count = 128;

#ifdef _WIN32
LPFN_ACCEPTEX native::accept = nullptr;
LPFN_CONNECTEX native::connect = nullptr;
LPFN_DISCONNECTEX native::disconnect = nullptr;
LPFN_GETACCEPTEXSOCKADDRS native::get_accept_socket_address = nullptr;

bool bind_extension_function(SOCKET s, GUID guid, PVOID *func)
{
    DWORD dwBytes;
    return SOCKET_ERROR != WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), func, sizeof(*func),
                                    &dwBytes, NULL, NULL);
}

HANDLE native::_cp = nullptr;
#else
std::vector<io_uring*> native::_io_uring_list;
thread_local io_uring* native::_io_uring = nullptr;
#endif

bool native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    socket dummy(protocol::tcp);
    if (!bind_extension_function(dummy.get_handle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&accept)))
        return false;
    if (!bind_extension_function(dummy.get_handle(), WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&connect)))
        return false;
    if (!bind_extension_function(dummy.get_handle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&disconnect)))
        return false;
    if (!bind_extension_function(dummy.get_handle(), WSAID_GETACCEPTEXSOCKADDRS,
								 reinterpret_cast<PVOID*>(&native::get_accept_socket_address)))
        return false;

	_cp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
#else
#endif
	if (option::auto_run)
	{
		run_io(option::thread_count);
	}
    return true;
}

void native::run_io(unsigned int num)
{
#ifndef _WIN32
	_io_uring_list.clear();
	_io_uring_list.resize(num);
#endif
	for (unsigned i = 0; i < num; ++i)
	{
		new std::thread(&native::io, i);
	}
}

void native::io(unsigned id)
{
#ifdef _WIN32
	context *context = nullptr;
	ULONG_PTR key = 0;
	DWORD numOfBytes = 0;
#else
	io_uring ring {};
	io_uring_cqe* cqe = nullptr;

	_io_uring = &ring;
	if (io_uring_queue_init(option::entry_count, &ring, 0))
		perror("io_uring_queue_init()");
	else
		_io_uring_list[id] = &ring;
#endif
	while (true)
	{
#ifdef _WIN32
		auto ret = GetQueuedCompletionStatus(_cp,
								  &numOfBytes,
								  &key,
								  reinterpret_cast<LPOVERLAPPED *>(&context),
								  option::timeout);
		if (ret)
		{
			if (!demux(context, numOfBytes, true))
				break;
		}
		else
		{
			const auto err = WSAGetLastError();
			switch (err) {
			case WAIT_TIMEOUT:
			case ERROR_OPERATION_ABORTED:
				break;
			default:
				if (!demux(context, numOfBytes, false))
					break;
				break;
			}
		}
#else
		if (io_uring_wait_cqe(&ring, &cqe))
			continue;

		if (!cqe->user_data)
		{
			perror("io_uring_wait_cqe()");
			io_uring_cqe_seen(&ring, cqe);
			break;
		}

		auto ctx = reinterpret_cast<context*>(cqe->user_data);
		if (cqe->res < 0)
		{
			if (!demux(ctx, static_cast<u_long>(cqe->res), false))
				break;
		}
		else
		{
			if (!demux(ctx, static_cast<u_long>(cqe->res), true))
				break;
		}


		io_uring_cqe_seen(&ring, cqe);
#endif
	}
}

bool native::demux(context* context, u_long transferred, bool success)
{
	switch (context->_io_type)
	{
	case io_type::accept:
		if (success)
		{
			auto listen_socket = reinterpret_cast<const socket*>(context->_token);
#ifdef _WIN32
			if (!observe(context->accept_socket.get()))
				return false;

			if (!context->accept_socket->set_option(options::level::socket, (net::option) SO_UPDATE_ACCEPT_CONTEXT, listen_socket->get_handle()))
				return false;
#else
			context->accept_socket->close();
			context->accept_socket->set_handle(static_cast<SOCKET>(transferred));
#endif

			sockaddr_in addr {};
			SOCKLEN len = sizeof(addr);
			if (SOCKET_ERROR == getpeername(context->accept_socket->get_handle(), reinterpret_cast<sockaddr*>(&addr), &len))
				perror("getpeername()");

			auto endpoint = endpoint::parse(addr);
			context->accept_socket->_remote_endpoint = endpoint;
			endpoint.set_port(listen_socket->get_local_endpoint()->get_port());
			context->accept_socket->_local_endpoint = endpoint;
		}
		context->completed(context, success);
		break;
	case io_type::connect:
		if (success)
		{
#ifdef _WIN32
			if (!static_cast<socket *>(context->_token)->set_option(options::level::socket,
																  (net::option) SO_UPDATE_CONNECT_CONTEXT, nullptr))
				return false;
#endif
		}
		context->completed(context, success);
		break;
	case io_type::disconnect:
		context->completed(context, success);
		break;
	case io_type::receive:
	case io_type::send:
		context->length = transferred;
		context->completed(context, success);
		break;
	default:
		return false;
	}
	return true;
}

bool native::observe(socket* sock)
{
#ifdef _WIN32
	auto r = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock->get_handle()), _cp, NULL, NULL);
	return nullptr != r;
#else
	return true;
#endif
}

#ifdef _WIN32
HANDLE native::get_handle()
{
	return _cp;
}
#else
io_uring* native::get_handle()
{
	static auto random = [](auto min, auto max) {
		static std::random_device rd;
		static std::mt19937_64 gen(rd());

		std::uniform_int_distribution<decltype(max)> dist(min, max);
		return dist(rd);
	};

	auto uring = _io_uring;
	if (!uring)
		uring = _io_uring_list[random(0, _io_uring_list.size()-1)];
	return uring;
}

#endif