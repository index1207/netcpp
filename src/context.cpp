#include "net/context.hpp"

using namespace net;

context::context() : accept_socket(std::make_unique<socket>())
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

context::~context()
{
}
