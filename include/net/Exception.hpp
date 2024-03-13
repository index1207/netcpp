#pragma once

#include <stdexcept>

namespace net
{
    class network_error : public std::exception
    {
    public:
        network_error(std::string_view msg);

        virtual char const* what() const override;
        int get_code() const;
    private:
        std::string _msg;
        int _error;
    };

}
