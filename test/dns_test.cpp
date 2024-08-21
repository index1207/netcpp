#include "gtest/gtest.h"
#include "net/dns.hpp"
#include "net/ip_address.hpp"

TEST(dns, get_host_name)
{
    EXPECT_NE(net::dns::get_host_name(), "");
}

TEST(dns, get_host_entry_url)
{
    auto youtubeEntry = net::dns::get_host_entry("www.youtube.com");
    EXPECT_GT(youtubeEntry.address_list.size(), 0);
    EXPECT_GT(youtubeEntry.alias_list.size(), 0);
}

TEST(dns, get_host_entry_address)
{
    auto entryByAddress = net::dns::get_host_entry(net::ip_address::loopback);
    EXPECT_GT(entryByAddress.address_list.size(), 0);
}

TEST(dns, get_host_entry_url_failure)
{
    auto entry = net::dns::get_host_entry(".com");
    EXPECT_EQ(entry.address_list.size(), 0);
}

TEST(dns, get_host_entry_address_failure)
{
    auto entry = net::dns::get_host_entry(net::ip_address::none);
    EXPECT_EQ(entry.address_list.size(), 0);
}
