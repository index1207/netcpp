#pragma once

#include <string>
#include <stdexcept>

namespace net
{
    class network_error : public std::exception
    {
    public:
        network_error(std::string_view msg);

        virtual const char* what() const noexcept override;
        int     getCode() const;
    private:
        std::string _msg;
        int _error;
    };

}
