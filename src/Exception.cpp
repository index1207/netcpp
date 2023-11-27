#include "PCH.h"
#include "Exception.hpp"

#include <string>
#include <system_error>

using namespace net;

network_error::network_error(std::string_view msg) : _msg(msg)
{
    _error = WSAGetLastError();
    const_cast<std::string&>(_msg) = std::format("{}: {}", _msg, std::system_category().message(_error));
}

char const* network_error::what() const
{
	return _msg.c_str();
}

int network_error::get_code() const {
    return _error;
}
