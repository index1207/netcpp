#include "PCH.h"
#include "Context.hpp"

using namespace net;

Context::Context()
{
    init();
}

void Context::init()
{
    OVERLAPPED::hEvent = 0;
    OVERLAPPED::Internal = 0;
    OVERLAPPED::InternalHigh = 0;
    OVERLAPPED::Offset = 0;
    OVERLAPPED::OffsetHigh = 0;

    _contextType = ContextType::None;

    isSuccess = true;
}

Context::~Context()
{
}
