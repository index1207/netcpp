#include "net/Context.hpp"

using namespace net;

Context::Context() : acceptSocket(std::make_unique<Socket>())
{
    init();
}

void Context::init()
{
    ZeroMemory(this, sizeof(OVERLAPPED));

    _contextType = ContextType::None;
}

Context::~Context()
{
}
