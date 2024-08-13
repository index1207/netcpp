#include "net/exception.hpp"
#include "net/native.hpp"

#include <format>

using namespace net;

network_exception::network_exception(std::string_view msg) : _msg(msg)
{
#ifdef _WIN32
    _error = WSAGetLastError();
#else
    _error = errno;
#endif
    const_cast<std::string &>(_msg) = std::format("[{}] {}: {}", _error, _msg, std::system_category().message(_error));
}

char const *network_exception::what() const noexcept
{
    return _msg.c_str();
}

int network_exception::get_code() const
{
    return _error;
}
