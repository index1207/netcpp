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
#else
    ZeroMemory(&_msg, sizeof(_msg));
#endif
	_io_type = io_type::none;
}

context::~context() = default;

void context::set_buffer(char* buffer, int offset, int count)
{
    if (!_buffer_list.empty())
        throw std::runtime_error("Can't be use with `buffer_list`");

    _buffer = std::span(buffer + offset, buffer + offset + count);
}

void context::set_buffer(std::span<char> buffer)
{
    if (!_buffer_list.empty())
        throw std::runtime_error("Can't be use with `buffer_list`");

    _buffer = buffer;
}

void context::add_data(char* buffer, int offset, int count)
{
#ifdef _WIN32
    _buffer_list.emplace_back(static_cast<ULONG>(count), buffer + offset);
#else
    _buffer_list.push_back({buffer + offset, static_cast<size_t>(count)});
#endif
}

void context::add_data(std::span<char> buffer)
{
#ifdef _WIN32
    _buffer_list.emplace_back(static_cast<ULONG>(buffer.size()), buffer.data());
#else
    _buffer_list.push_back({buffer.data(), buffer.size()});
#endif
}
