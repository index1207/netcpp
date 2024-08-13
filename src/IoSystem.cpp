#include "net/IoSystem.hpp"

#include <iostream>
#include <thread>

#include "net/exception.hpp"
#include "net/context.hpp"
#include "net/native.hpp"
#include "net/socket.hpp"

using namespace net;

#ifdef _WIN32
bool Option::Autorun = true;
unsigned long Option::Timeout = INFINITE;
unsigned Option::ThreadCount = std::thread::hardware_concurrency();

IoSystem::IoSystem()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

    if (Option::Autorun)
    {
        for (unsigned i = 0; i < Option::ThreadCount; ++i) {
            new std::thread([this] {
                while (true) worker();
            });
        }
    }
}

IoSystem::~IoSystem()
{
    CancelIo(_hcp);
}

void IoSystem::push(SOCKET s)
{
	if(INVALID_HANDLE_VALUE == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL))
        throw network_exception("CreateIoCompletionPort");
}

void IoSystem::dispatch(context* context, DWORD numOfBytes, bool isSuccess) {
    switch (context->_contextType) {
        case ContextType::Accept:
            if (isSuccess) {
                this->push(context->acceptSocket->get_handle());
                if (!context->acceptSocket->set_option(options::level::socket, (option) SO_UPDATE_ACCEPT_CONTEXT,
                                                       _listeningSocket->get_handle()))
                    throw net::network_exception("setSocketOption()");
            }
            context->completed(context, isSuccess);
            break;
        case ContextType::Connect:
            if (isSuccess) {
                if (!static_cast<socket *>(context->token)->set_option(options::level::socket,
                                                                       (option) SO_UPDATE_CONNECT_CONTEXT, nullptr))
                    throw net::network_exception("setSocketOption()");
            }
            context->completed(context, isSuccess);
            break;
        case ContextType::Disconnect:
            context->completed(context, isSuccess);
            break;
        case ContextType::Receive:
        case ContextType::Send:
            if(isSuccess) {
                context->length = numOfBytes;
            }
            context->completed(context, isSuccess);
            break;
        default:
            break;
    }
}

DWORD IoSystem::worker() {
    context *context = nullptr;
    ULONG_PTR key = 0;
    DWORD numOfBytes = 0;
    if (GetQueuedCompletionStatus(_hcp,
                                  &numOfBytes,
                                  &key,
                                  reinterpret_cast<LPOVERLAPPED *>(&context),
                                  Option::Timeout)) {
        dispatch(context, numOfBytes, true);
    }
    else
    {
        const auto err = WSAGetLastError();
        switch (err) {
            case WAIT_TIMEOUT:
            case ERROR_OPERATION_ABORTED:
                break;
            default:
                dispatch(context, numOfBytes, false);
                break;
        }
    }
    return 0;
}

HANDLE IoSystem::getHandle() {
    return _hcp;
}
#endif
