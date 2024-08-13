#pragma once

#include <stdexcept>
#include <string>

namespace net
{
class network_exception : public std::exception
{
  public:
    explicit network_exception(std::string_view msg);

    [[nodiscard]] const char *what() const noexcept override;
    [[nodiscard]] int get_code() const;

  private:
    std::string _msg;
    int _error;
};

} // namespace net
