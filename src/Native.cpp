#include "net/Native.hpp"
#include "net/Socket.hpp"
#include "net/Context.hpp"

#include <stdexcept>
#include <thread>

using namespace net;

#ifdef _WIN32
enum
{
    CK_RIO = 0x400
};

bool Native::Option::Autorun = true;
unsigned long Native::Option::Timeout = INFINITE;
unsigned Native::Option::ThreadCount = std::thread::hardware_concurrency();

LPFN_ACCEPTEX Native::acceptEx = nullptr;
LPFN_CONNECTEX Native::connectEx = nullptr;
LPFN_DISCONNECTEX Native::disconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS Native::getAcceptExSockAddr = nullptr;

ULONG Native::Option::ResultSize = 0x100;
ULONG Native::Option::SendRequestQueSize = 0x20;
ULONG Native::Option::ReceiveRequestQueSize = 0x4;
ULONG Native::Option::MaxClientCount = 0x1000;

RIO_EXTENSION_FUNCTION_TABLE Native::rioTable { 0, };
thread_local RIO_CQ Native::completionQue = nullptr;

std::function<void(bool)> Native::onExitIo = [](bool){ };

HANDLE Native::_hcp = INVALID_HANDLE_VALUE;

bool bindIocpFunction(const Socket& sock, GUID guid, PVOID* func)
{
	DWORD dwBytes = 0;
    return SOCKET_ERROR != WSAIoctl(sock.getHandle(),
                                    SIO_GET_EXTENSION_FUNCTION_POINTER,
                                    &guid, sizeof(GUID),
                                    func, sizeof(*func),
                                    &dwBytes, NULL, NULL);
}

bool bindRioFunctionTable(const Socket& sock, PVOID* table)
{
    GUID functionTableId = WSAID_MULTIPLE_RIO;
    DWORD dwBytes = 0;
    return SOCKET_ERROR != WSAIoctl(sock.getHandle(),
                                    SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER,
                                    &functionTableId, sizeof(GUID),
                                    table, sizeof(RIO_EXTENSION_FUNCTION_TABLE),
                                    &dwBytes, NULL, NULL);
}

#endif

bool Native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    Socket dummy(Protocol::Tcp);
    // Bind extension functions.
	if (!bindIocpFunction(dummy, WSAID_ACCEPTEX, reinterpret_cast<PVOID*>(&acceptEx)))
        return false;
	if (!bindIocpFunction(dummy, WSAID_CONNECTEX, reinterpret_cast<PVOID*>(&connectEx)))
        return false;
	if (!bindIocpFunction(dummy, WSAID_DISCONNECTEX, reinterpret_cast<PVOID*>(&disconnectEx)))
        return false;
	if (!bindIocpFunction(dummy, WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID*>(&Native::getAcceptExSockAddr)))
        return false;

    // Bind Registered I/O Function table.
    if (!bindRioFunctionTable(dummy, reinterpret_cast<PVOID*>(&rioTable)))
        return false;

    _hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL); // Create new CP.
    if (Option::Autorun)
    {
        for (unsigned i = 0; i < Option::ThreadCount; ++i) {
            new std::thread(&Native::ioWorker); // Launch ioWork threads.
        }
    }
#else
#endif
    return true;
}

#ifdef _WIN32
void Native::ioWorker()
{
    bool error = false;

    Context* context = nullptr;
    ULONG_PTR key = 0;
    DWORD numOfBytes = 0;

    net::Context rioCtx;
    RIO_NOTIFICATION_COMPLETION completion;
    completion.Type = RIO_IOCP_COMPLETION;
    completion.Iocp.IocpHandle = _hcp;
    completion.Iocp.CompletionKey = reinterpret_cast<void*>(CK_RIO);
    completion.Iocp.Overlapped = &rioCtx;

    auto ioResults = new RIORESULT[Option::ResultSize] { 0, };

    completionQue = rioTable.RIOCreateCompletionQueue(Option::getCompletionQueSize(), &completion);
    while (completionQue)
    {
        if (rioTable.RIONotify(completionQue) != ERROR_SUCCESS)
            break;

        if (!GetQueuedCompletionStatus(_hcp,
                                      &numOfBytes,
                                      &key,
                                      reinterpret_cast<LPOVERLAPPED*>(&context),
                                      Option::Timeout))
        {
            const auto errCode = WSAGetLastError();
            switch (errCode) {
                case WAIT_TIMEOUT:
                case ERROR_OPERATION_ABORTED:
                    break;
                default:
                    error |= handleIocpEvent(context, false);
                    break;
            }
        }
        if (key != CK_RIO && context != nullptr)
            error |= handleIocpEvent(context, true);

        ZeroMemory(ioResults, sizeof(ioResults));
        auto length = rioTable.RIODequeueCompletion(completionQue, ioResults, Option::ResultSize);
        if (length == 0 || RIO_CORRUPT_CQ == length)
            break;

        if (rioTable.RIONotify(completionQue) != ERROR_SUCCESS)
            break;

        for (ULONG i = 0; i < length; ++i)
        {
            auto ctx = reinterpret_cast<net::Context*>(ioResults[i].RequestContext);
            error |= handleRioEvent(ctx, ioResults[i].BytesTransferred);
        }

        if (!error) break;
    }
    delete[] ioResults;
    Native::onExitIo(error);
}

bool Native::addToCompletionPort(SOCKET sock) {
    return INVALID_HANDLE_VALUE != ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), _hcp, NULL, NULL);
}

bool Native::handleIocpEvent(Context *context, bool success) {
    switch (context->_contextType) {
        case ContextType::Accept:
            if (success) {
                if (!context->acceptSocket->setOption(OptionLevel::Socket,
                                                      OptionName::UpdateAcceptContext,
                                                      reinterpret_cast<Socket*>(context->token)->getHandle()))
                    return false;

                if (!addToCompletionPort(context->acceptSocket->getHandle()))
                    return false;

                RIO_RQ requestQue = rioTable.RIOCreateRequestQueue(context->acceptSocket->getHandle(),
                                                                   Option::ReceiveRequestQueSize, 1,
                                                                   Option::SendRequestQueSize, 1,
                                                                   completionQue, completionQue,
                                                                   nullptr);
                if (requestQue == RIO_INVALID_RQ)
                    return false;

                context->acceptSocket->_requestQue = requestQue;
            }
            context->completed(context, success);
            break;
        case ContextType::Connect:
            if (success) {
                return static_cast<Socket*>(context->token)->setOption(OptionLevel::Socket, (OptionName)SO_UPDATE_CONNECT_CONTEXT, nullptr);
            }
            context->completed(context, success);
            break;
        case ContextType::Disconnect:
            context->completed(context, success);
            break;
        default:
            break;
    }
    return true;
}

bool Native::handleRioEvent(Context* context, ULONG transferred)
{
    auto sock = static_cast<net::Socket*>(context->token);
    if (transferred == 0)
    {
        auto disconnectCompl = [](net::Context* context, bool success) {
            delete context;
        };
        auto disconnectCtx = new net::Context;
        disconnectCtx->completed = disconnectCompl;
        if (!sock->disconnect(disconnectCtx))
            disconnectCompl(disconnectCtx, false);
    }

    switch (context->_contextType)
    {
        case ContextType::Send:
            context->length = transferred;
            context->completed(context, true);
            break;
        case ContextType::Receive:
            break;
            context->length = transferred;
            context->completed(context, true);
        default:
            return false;
            break;
    }
    return true;
}
#endif
