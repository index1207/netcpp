#include "PCH.h"
#include "IoSystem.hpp"

#include <iostream>
#include <thread>
#include <windef.h>

#include "Context.hpp"
#include "Native.hpp"
#include "Socket.hpp"

using namespace net;

bool Option::Autorun = true;
unsigned long long Option::Timeout = INFINITE;
unsigned Option::ThreadCount = std::thread::hardware_concurrency();

IoSystem::IoSystem()
{
	_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

#ifndef SINGLE_ONLY
    if (Option::Autorun)
    {
        std::lock_guard lock(mtx);
        for (unsigned i = 0; i < Option::ThreadCount; ++i) {
            new std::thread([this] {
                while (true) worker();
            });
        }
    }
#endif
}

IoSystem::~IoSystem()
{
    CancelIo(_hcp);
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
                this->push(context->acceptSocket->getHandle());
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

DWORD IoSystem::worker() {
    Context *context = nullptr;
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
        if (context == nullptr)
            return -1;

        auto err = WSAGetLastError();
        switch (err) {
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
