#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>

#include "Extension.hpp"
#include "NetCore.hpp"
#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "Socket.hpp"
#include "SocketAsyncEvent.hpp"

#ifdef USE_AGENT
#include "Agent.hpp"
#include "Listener.hpp"
#endif // USE_AGENT