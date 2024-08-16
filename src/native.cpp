#include "net/native.hpp"
#include "net/socket.hpp"
#include "net/context.hpp"

#include <stdexcept>
#include <thread>

using namespace net;

std::atomic<bool> native::_running = false;

#ifdef _WIN32
bool native::option::auto_run = true;
unsigned long native::option::timeout = INFINITE;
unsigned native::option::thread_count = std::thread::hardware_concurrency();

LPFN_ACCEPTEX native::accept = nullptr;
LPFN_CONNECTEX native::connect = nullptr;
LPFN_DISCONNECTEX native::disconnect = nullptr;
LPFN_GETACCEPTEXSOCKADDRS native::get_accept_socket_address = nullptr;

HANDLE native::_cp = nullptr;

bool bind_extension_function(SOCKET s, GUID guid, PVOID *func)
{
    DWORD dwBytes;
    return SOCKET_ERROR != WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), func, sizeof(*func),
                                    &dwBytes, NULL, NULL);
}
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

	if (option::auto_run)
	{
		_running = true;
		for (unsigned i = 0; i < option::thread_count; ++i)
		{
			new std::thread(&native::io);
		}
	}
#else
#endif
    return true;
}
void native::io()
{
	while (_running)
	{
#ifdef _WIN32
		context *context = nullptr;
		ULONG_PTR key = 0;
		DWORD numOfBytes = 0;
		if (GetQueuedCompletionStatus(_cp,
									  &numOfBytes,
									  &key,
									  reinterpret_cast<LPOVERLAPPED *>(&context),
									  option::timeout)) {
			if (!demux(context, numOfBytes, true))
				_running = false;
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
					_running = false;
				break;
			}
		}
#endif
	}
}

bool native::demux(context* context, u_long transferred, bool success)
{
#ifdef _WIN32
	switch (context->_io_type)
	{
	case io_type::accept:
		if (success) {
			if (!add_to_cp(context->accept_socket.get()))
				return false;

			if (!context->accept_socket->set_option(options::level::socket, (net::option) SO_UPDATE_ACCEPT_CONTEXT,
													reinterpret_cast<const socket*>(context->_token)->get_handle()))
				return false;
		}
		context->completed(context, success);
		break;
	case io_type::connect:
		if (success) {
			if (!static_cast<socket *>(context->_token)->set_option(options::level::socket,
																  (net::option) SO_UPDATE_CONNECT_CONTEXT, nullptr))
				return false;
		}
		context->completed(context, success);
		break;
	case io_type::disconnect:
		context->completed(context, success);
		break;
	case io_type::receive:
	case io_type::send:
		if(success) {
			context->length = transferred;
		}
		context->completed(context, success);
		break;
	default:
		return false;
	}
#endif
	return true;
}

#ifdef _WIN32
bool native::add_to_cp(class socket* sock)
{
	auto ret = CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock->get_handle()), _cp, NULL, NULL);
	return ret != nullptr;
}
#endif
