#include "net/native.hpp"
#include "net/Context.hpp"
#include "net/Socket.hpp"

#include <stdexcept>
#include <thread>

using namespace net;

#ifdef _WIN32
enum
{
    CK_RIO = 0x400
};

bool native::option::Autorun = true;
unsigned long native::option::Timeout = INFINITE;
unsigned native::option::ThreadCount = std::thread::hardware_concurrency();

LPFN_ACCEPTEX native::acceptex = nullptr;
LPFN_CONNECTEX native::connectex = nullptr;
LPFN_DISCONNECTEX native::disconnectex = nullptr;
LPFN_GETACCEPTEXSOCKADDRS native::get_acceptex_socket_address = nullptr;

ULONG native::option::ResultSize = 0x100;
ULONG native::option::SendRequestQueSize = 0x20;
ULONG native::option::ReceiveRequestQueSize = 0x4;
ULONG native::option::MaxClientCount = 0x1000;

RIO_EXTENSION_FUNCTION_TABLE native::rio{
    0,
};
thread_local RIO_CQ native::completionQue = nullptr;

std::function<void(bool)> native::onExitIo = [](bool) {};

HANDLE native::_hcp = INVALID_HANDLE_VALUE;

bool bindIocpFunction(const net::socket &sock, GUID guid, PVOID *func)
{
    DWORD dwBytes = 0;
    return SOCKET_ERROR != WSAIoctl(sock.get_handle(), SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), func,
                                    sizeof(*func), &dwBytes, NULL, NULL);
}

bool bindRioFunctionTable(const net::socket &sock, PVOID *table)
{
    GUID functionTableId = WSAID_MULTIPLE_RIO;
    DWORD dwBytes = 0;
    return SOCKET_ERROR != WSAIoctl(sock.get_handle(), SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &functionTableId,
                                    sizeof(GUID), table, sizeof(RIO_EXTENSION_FUNCTION_TABLE), &dwBytes, NULL, NULL);
}

#endif

bool native::initialize()
{
#ifdef _WIN32
    WSADATA wsaData{};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;

    net::socket dummy(protocol::tcp);
    // Bind extension functions.
    if (!bindIocpFunction(dummy, WSAID_ACCEPTEX, reinterpret_cast<PVOID *>(&acceptex)))
        return false;
    if (!bindIocpFunction(dummy, WSAID_CONNECTEX, reinterpret_cast<PVOID *>(&connectex)))
        return false;
    if (!bindIocpFunction(dummy, WSAID_DISCONNECTEX, reinterpret_cast<PVOID *>(&disconnectex)))
        return false;
    if (!bindIocpFunction(dummy, WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID *>(&native::get_acceptex_socket_address)))
        return false;

    // Bind Registered I/O Function table.
    if (!bindRioFunctionTable(dummy, reinterpret_cast<PVOID *>(&rio)))
        return false;

    _hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL); // Create new CP.
    if (option::Autorun)
    {
        for (unsigned i = 0; i < option::ThreadCount; ++i)
        {
            new std::thread(&native::io_worker); // Launch ioWork threads.
        }
    }
#else
#endif
    return true;
}

#ifdef _WIN32
void native::io_worker()
{
    bool error = false;

    net::context *context = nullptr;
    ULONG_PTR key = 0;
    DWORD numOfBytes = 0;

    net::context rioCtx;
    RIO_NOTIFICATION_COMPLETION completion;
    completion.Type = RIO_IOCP_COMPLETION;
    completion.Iocp.IocpHandle = _hcp;
    completion.Iocp.CompletionKey = reinterpret_cast<void *>(CK_RIO);
    completion.Iocp.Overlapped = &rioCtx;

    auto ioResults = new RIORESULT[option::ResultSize];

    completionQue = rio.RIOCreateCompletionQueue(option::getCompletionQueSize(), &completion);
    while (completionQue)
    {
        if (rio.RIONotify(completionQue) != ERROR_SUCCESS)
            break;

        if (!GetQueuedCompletionStatus(_hcp, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED *>(&context),
                                       option::Timeout))
        {
            const auto errCode = WSAGetLastError();
            switch (errCode)
            {
            case WAIT_TIMEOUT:
            case ERROR_OPERATION_ABORTED:
                break;
            default:
                error |= handle_iocp_event(context, false);
                break;
            }
        }
        if (key != CK_RIO && context != nullptr)
        {
            error |= handle_iocp_event(context, true);
            continue;
        }

        ZeroMemory(ioResults, sizeof(ioResults));
        auto length = rio.RIODequeueCompletion(completionQue, ioResults, option::ResultSize);
        if (length == 0 || RIO_CORRUPT_CQ == length)
            break;

        if (rio.RIONotify(completionQue) != ERROR_SUCCESS)
            break;

        for (ULONG i = 0; i < length; ++i)
        {
            auto ctx = reinterpret_cast<net::context *>(ioResults[i].RequestContext);
            error |= handle_rio_event(ctx, ioResults[i].BytesTransferred);
        }

        if (!error)
            break;
    }
    delete[] ioResults;
    native::onExitIo(error);
}

bool native::register_to_iocp(SOCKET sock)
{
    return INVALID_HANDLE_VALUE != ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), _hcp, NULL, NULL);
}

bool native::handle_iocp_event(context *context, bool success)
{
    switch (context->type)
    {
    case context::io_type::accept:
        if (success)
        {
            if (!context->accept_socket->set_option(options::level::socket, net::option::accept_context,
                                                   reinterpret_cast<net::socket *>(context->token)->get_handle()))
                return false;

            if (!register_to_iocp(context->accept_socket->get_handle()))
                return false;

            RIO_RQ requestQue =
                rio.RIOCreateRequestQueue(context->accept_socket->get_handle(), option::ReceiveRequestQueSize, 1,
                                               option::SendRequestQueSize, 1, completionQue, completionQue, nullptr);
            if (requestQue == RIO_INVALID_RQ)
                return false;

            context->accept_socket->_request_queue = requestQue;
        }
        context->completed(context, success);
        break;
    case context::io_type::connect:
        if (success)
        {
            auto sock = static_cast<net::socket *>(context->token);
            if (!sock->set_option(net::options::level::socket, (net::option)SO_UPDATE_CONNECT_CONTEXT, nullptr))
                return false;
        }
        context->completed(context, success);
        break;
    case context::io_type::disconnect:
        context->completed(context, success);
        break;
    default:
        break;
    }
    return true;
}

bool native::handle_rio_event(context *context, ULONG transferred)
{
    switch (context->type)
    {
    case context::io_type::send:
        context->length = transferred;
        context->completed(context, true);
        break;
    case context::io_type::receive:
        break;
        context->length = transferred;
        context->completed(context, true);
    default:
        return false;
        break;
    }
    return true;
}
HANDLE native::get_handle()
{
    return _hcp;
}
#endif