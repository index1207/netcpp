#pragma once

#include <string>
#include <span>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <mutex>
#include <memory>
#include <functional>
#include <future>
#include <queue>

#include <cmath>
#include <cassert>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#ifndef _DEBUG
#pragma warning(disable: 6011)
#undef assert
#define assert(expr) \
{ \
	if(!(expr)) \
	{ \
		int* ptr = nullptr; \
		*ptr = 0xABCDEF; \
	} \
}
#endif // _DEBUG
