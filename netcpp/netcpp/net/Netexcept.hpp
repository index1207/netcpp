#pragma once

#include <stdexcept>

class network_error : public std::exception
{
public:
	network_error(std::string_view msg);

	virtual char const* what() const;
private:
	std::string _msg;
	std::string _cause;
};

