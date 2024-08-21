#include "net/exception.hpp"
#include "net/native.hpp"

#include <sstream>

using namespace net;

network_exception::network_exception(std::string_view msg)
{
#ifdef _WIN32
    _error = WSAGetLastError();
#else
    _error = errno;
#endif
	std::stringstream ss;
	ss << msg << ": " << std::system_category().message(_error) << " [" << _error << "]";
	const_cast<std::string &>(_msg) = ss.str();
}

char const *network_exception::what() const noexcept
{
    return _msg.c_str();
}

int network_exception::get_code() const
{
    return _error;
}
