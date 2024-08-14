#include "net/context.hpp"

using namespace net;

context::context() : accept_socket(std::make_unique<async_socket>())
{
    buffer = nullptr;
#ifdef _WIN32
    _buffer_id = RIO_INVALID_BUFFERID;
#endif
    init();
}

void context::init()
{
#ifdef _WIN32
    ZeroMemory(this, sizeof(OVERLAPPED));
#endif
	type = io_type::none;
}

context::~context()
{
#ifdef _WIN32
    if (buffer)
    {
		native::rio.RIODeregisterBuffer(_buffer_id);
        VirtualFreeEx(GetCurrentProcess(), buffer, 0, MEM_RELEASE);
    }
    CancelIoEx(native::get_handle(), reinterpret_cast<OVERLAPPED*>(this));
#endif
}

bool context::create_buffer(u_long size)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    const unsigned __int64 granularity = systemInfo.dwAllocationGranularity;

    if (size % granularity == 0)
    {
        buffer = reinterpret_cast<char*>(VirtualAllocEx(GetCurrentProcess(), nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
        if (!buffer)
            return false;

        _buffer_id = native::rio.RIORegisterBuffer(buffer, size);
        if (_buffer_id == RIO_INVALID_BUFFERID)
            return false;
    }
    else return false;
    return true;
}