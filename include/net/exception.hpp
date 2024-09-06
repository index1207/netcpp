#pragma once

#include <stdexcept>
#include <string>

namespace net
{
    class exception final : public std::exception
    {
    public:
        explicit exception(std::string_view msg);

        [[nodiscard]] const char* what() const noexcept override;
        [[nodiscard]] static int get_code();

    private:
        std::string _msg;
    };
}
