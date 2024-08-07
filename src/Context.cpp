#include "net/Context.hpp"

using namespace net;

Context::Context() : acceptSocket(std::make_unique<Socket>())
{
    init();
}

void Context::init()
{
#ifdef _WIN32
    ZeroMemory(this, sizeof(OVERLAPPED));
#endif
    _contextType = ContextType::None;
}

Context::~Context()
{
}
