#include "net/context.hpp"

#include <stdexcept>

using namespace net;

context::context() :
#if _WIN32
	  OVERLAPPED(),
#endif
    completed([](context *, bool) {}), length(0), _token(nullptr), _io_type(io_type::none)
{
    init();
}

void context::init()
{
#ifdef _WIN32
    ZeroMemory(this, sizeof(OVERLAPPED));
#endif
	_io_type = io_type::none;
}

context::~context() = default;

void context::set_buffer(char* buffer, int offset, int count)
{
    if (buffer_list.has_value())
        throw std::runtime_error("Can't be use with `buffer_list`");

    _buffer = std::span(buffer + offset, buffer + offset + count);
}

void context::set_buffer(std::span<char> buffer)
{
    if (buffer_list.has_value())
        throw std::runtime_error("Can't be use with `buffer_list`");

    _buffer = buffer;
}
