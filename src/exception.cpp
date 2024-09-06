#include "net/exception.hpp"
#include "net/native.hpp"

#include <sstream>

using namespace net;

exception::exception(std::string_view msg)
{
    const auto error =
#ifdef _WIN32
        WSAGetLastError();
#else
        errno;
#endif
	std::stringstream ss;
	ss << msg << ": " << std::system_category().message(error);
	_msg = ss.str();
}

char const *exception::what() const noexcept
{
    return _msg.c_str();
}

int exception::get_code()
{
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
