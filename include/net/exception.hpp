#pragma once

#include <string>

#include "net/export.hpp"

namespace net
{
    class exception final : public std::exception
    {
    public:
        NETCPP_API explicit exception(std::string_view msg);

        [[nodiscard]] NETCPP_API const char* what() const noexcept override;
        [[nodiscard]] NETCPP_API static int get_code();

    private:
        std::string _msg;
    };
}
