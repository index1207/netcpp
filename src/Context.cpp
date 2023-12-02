#include "PCH.h"
#include "Context.hpp"

using namespace net;

Context::Context()
{
    memset(this, 0, sizeof(WSAOVERLAPPED));
    this->socketError = SocketError::Error;
}
