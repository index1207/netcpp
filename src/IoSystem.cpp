#include "PCH.h"
#include "IoSystem.hpp"

#include <thread>
#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"
#include "Socket.hpp"

using namespace net;

IoSystem net::ioSystem;
const Socket* IoSystem::_listeningSocket;

IoSystem::IoSystem()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

    std::lock_guard lock(mtx);
	for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        DWORD id;
        CreateThread(nullptr, 0, worker, _hcp, 0, &id);
    }
}

IoSystem::~IoSystem()
{
}

void IoSystem::push(SOCKET s)
{
	if(INVALID_HANDLE_VALUE == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), _hcp, NULL, NULL))
        throw network_error("CreateIoCompletionPort");
}

void IoSystem::dispatch(Context* context, DWORD numOfBytes, bool isSuccess) {
    switch (context->_contextType) {
        case ContextType::Accept:
            if(isSuccess) {
                ioSystem.push(context->acceptSocket->getHandle());
                context->acceptSocket->setSocketOption(OptionLevel::Socket, (OptionName)SO_UPDATE_ACCEPT_CONTEXT, _listeningSocket->getHandle());
            }
            context->completed(context, isSuccess);
            break;
        case ContextType::Connect:
            if(isSuccess) {
                context->acceptSocket->setSocketOption(OptionLevel::Socket, (OptionName) SO_UPDATE_CONNECT_CONTEXT,nullptr);
            }
            context->completed(context, isSuccess);
            break;
        case ContextType::Disconnect:
            context->completed(context, isSuccess);
            break;
        case ContextType::Receive:
        case ContextType::Send:
            if(isSuccess) {
                context->length.store(numOfBytes);
            }
            context->completed(context, isSuccess);
            break;
        default:
            break;
    }
}

DWORD IoSystem::worker(HANDLE cp) {
    Context *context = nullptr;
    ULONG_PTR key = 0;
    DWORD numOfBytes = 0;
    while(true) {
        if (GetQueuedCompletionStatus(cp,
                                      &numOfBytes,
                                      &key,
                                      reinterpret_cast<LPOVERLAPPED *>(&context),
                                      INFINITE)) {
            dispatch(context, numOfBytes, true);
        } else {
            if (context == nullptr)
                break;

            auto err = WSAGetLastError();
            switch (err) {
                case ERROR_OPERATION_ABORTED:
                    break;
                default:
                    dispatch(context, numOfBytes, false);
                    break;
            }
        }
    }
    return 0;
}
