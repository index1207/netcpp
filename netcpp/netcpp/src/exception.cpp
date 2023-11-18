#include "pch.h"
#include "..\net\exception.hpp"

#include <string>
#include <system_error>

network_error::network_error(std::string_view msg) : _msg(msg)
{
}

char const* network_error::what() const
{
	const auto err = WSAGetLastError();
	const_cast<std::string&>(_msg) = std::format("[{}] {}: {}", err, _msg, std::system_category().message(err));
	return _msg.c_str();
}
