#include "net/Native.hpp"
#include "net/Socket.hpp"
#include "net/Context.hpp"

#include <stdexcept>
#include <thread>

using namespace net;

#ifdef _WIN32
bool Native::Option::Autorun = true;
unsigned long Native::Option::Timeout = INFINITE;
unsigned Native::Option::ThreadCount = std::thread::hardware_concurrency();

LPFN_ACCEPTEX Native::acceptEx = nullptr;
LPFN_CONNECTEX Native::connectEx = nullptr;
LPFN_DISCONNECTEX Native::disconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS Native::getAcceptExSockAddr = nullptr;

thread_local RIO_CQ Native::completionQue = nullptr;

std::function<void(bool)> Native::onExitIo = [](bool){ };

HANDLE Native::_hcp = INVALID_HANDLE_VALUE;

bool bindExtensionFunction(SOCKET s, GUID guid, PVOID* func)
{
	DWORD dwBytes;
	return SOCKET_ERROR != WSAIoctl(s,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(GUID),
		func, sizeof(*func),
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
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_ACCEPTEX, reinterpret_cast<PVOID *>(&acceptEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_CONNECTEX, reinterpret_cast<PVOID *>(&connectEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_DISCONNECTEX, reinterpret_cast<PVOID *>(&disconnectEx)))
        return false;
	if(!bindExtensionFunction(dummy.getHandle(), WSAID_GETACCEPTEXSOCKADDRS, reinterpret_cast<PVOID *>(&Native::getAcceptExSockAddr)))
        return false;

    _hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    if (Option::Autorun)
    {
        for (unsigned i = 0; i < Option::ThreadCount; ++i) {
            new std::thread(&Native::ioWorker);
        }
    }
#else
#endif
    return true;
}

void Native::ioWorker() {

    bool error = false;
    while (true)
    {
        Context *context = nullptr;
        ULONG_PTR key = 0;
        DWORD numOfBytes = 0;
        if (GetQueuedCompletionStatus(_hcp,
                                      &numOfBytes,
                                      &key,
                                      reinterpret_cast<LPOVERLAPPED *>(&context),
                                      Option::Timeout)) {
            error = handleEvent(context, numOfBytes, true);
        }
        else
        {
            const auto errCode = WSAGetLastError();
            switch (errCode) {
                case WAIT_TIMEOUT:
                case ERROR_OPERATION_ABORTED:
                    break;
                default:
                    error = handleEvent(context, numOfBytes, false);
                    break;
            }
        }

        if (error) break;
    }
    Native::onExitIo(error);
}

bool Native::addToCompletionPort(SOCKET sock) {
    return INVALID_HANDLE_VALUE != ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), _hcp, NULL, NULL);
}

bool Native::handleEvent(Context *context, DWORD bytes, bool success) {
    switch (context->_contextType) {
        case ContextType::Accept:
            if (success) {
                addToCompletionPort(context->acceptSocket->getHandle());
                return context->acceptSocket->setOption(OptionLevel::Socket,
                                                        static_cast<OptionName>(SO_UPDATE_ACCEPT_CONTEXT),
                                                        static_cast<Socket*>(context->token)->getHandle());
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
        case ContextType::Receive:
        case ContextType::Send:
            if(success) {
                context->length = bytes;
            }
            context->completed(context, success);
            break;
        default:
            break;
    }
    return true;
}
