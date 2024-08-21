#include "net/context.hpp"

using namespace net;

context::context() :
#if _WIN32
	  OVERLAPPED(),
#endif
	  accept_socket(std::make_unique<socket>()), completed([](context *, bool) {}), length(0), buffer(), _token(nullptr), _io_type(io_type::none)
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
