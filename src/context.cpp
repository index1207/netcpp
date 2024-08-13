#include "net/context.hpp"

using namespace net;

context::context() : acceptSocket(std::make_unique<socket>())
{
    buffer = nullptr;
#ifdef _WIN32
    _bufferId = RIO_INVALID_BUFFERID;
#endif
    init();
}

void context::init()
{
#ifdef _WIN32
    ZeroMemory(this, sizeof(OVERLAPPED));
#endif
    _contextType = ContextType::None;
}

context::~context()
{
#ifdef _WIN32
    if (buffer)
    {
        Native::rioTable.RIODeregisterBuffer(_bufferId);
        VirtualFreeEx(GetCurrentProcess(), buffer, 0, MEM_RELEASE);
    }
#endif
}

#ifdef _WIN32
bool Context::createBuffer(DWORD size)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    const unsigned __int64 granularity = systemInfo.dwAllocationGranularity;

    if (size % granularity == 0)
    {
        buffer = reinterpret_cast<char*>(VirtualAllocEx(GetCurrentProcess(), nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (!buffer)
            return false;

        _bufferId = Native::rioTable.RIORegisterBuffer(buffer, size);
        if (_bufferId == RIO_INVALID_BUFFERID)
            return false;
    }
    else return false;
    return true;
}
#endif