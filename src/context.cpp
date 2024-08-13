#include "net/context.hpp"

using namespace net;

context::context() : acceptSocket(std::make_unique<socket>())
{
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
}
