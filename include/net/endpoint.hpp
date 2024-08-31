#pragma once

#include <string>

#include "net/ip_address.hpp"
#include "net/native.hpp"

namespace net
{
    class endpoint
    {
    public:
        endpoint() = default;
        endpoint(ip_address address, unsigned short port);
    public:
        [[nodiscard]] const ip_address& get_address() const;
        [[nodiscard]] int get_port() const;
        void set_address(ip_address address);
        void set_port(unsigned short port);

        [[nodiscard]] std::string to_string() const;
    public:
        static endpoint parse(sockaddr_in addr);
        static bool try_parse(std::string_view s, endpoint* ep);
    public:
        bool operator==(const endpoint& endpoint) const;
        bool operator==(endpoint&& endpoint) const;
    private:
        ip_address _address;
    };
}
