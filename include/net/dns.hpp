#pragma once

#include <string>
#include <vector>

#include "net/export.hpp"

namespace net
{
    struct host_entry
    {
        std::vector<class ip_address> address_list;
        std::vector<std::string> alias_list;
        std::string host_name;
    };

    class NETCPP_API dns
    {
    public:
        static std::string get_host_name();
        static host_entry get_host_entry(std::string_view hostname);
        static host_entry get_host_entry(ip_address host);
    };
}
