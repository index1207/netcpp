#include "net/Exception.hpp"
#include "net/Native.hpp"

#include <format>

using namespace net;

network_error::network_error(std::string_view msg) : _msg(msg)
{
#ifdef _WIN32
    _error = WSAGetLastError();
#else
    _error = errno;
#endif
    const_cast<std::string&>(_msg) = std::format("[{}] {}: {}", _error, _msg, std::system_category().message(_error));
}

char const* network_error::what() const noexcept
{
	return _msg.c_str();
}

int network_error::getCode() const {
    return _error;
}
