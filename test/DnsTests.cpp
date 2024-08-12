#include "gtest/gtest.h"
#include "net/Dns.hpp"
#include "net/IpAddress.hpp"

TEST(Dns, getHostName)
{
    EXPECT_NE(net::Dns::getHostName(), "");
}

TEST(Dns, getHostEntry_url)
{
    auto youtubeEntry = net::Dns::getHostEntry("www.youtube.com");
    EXPECT_GT(youtubeEntry.address_list.size(), 0);
    EXPECT_GT(youtubeEntry.alias_list.size(), 0);
}

TEST(Dns, getHostEntry_address)
{
    auto entryByAddress = net::Dns::getHostEntry(net::IpAddress::Loopback);
    EXPECT_GT(entryByAddress.address_list.size(), 0);
}

TEST(Dns, getHostEntry_url_failure)
{
    auto entry = net::Dns::getHostEntry(".com");
    EXPECT_EQ(entry.address_list.size(), 0);
}

TEST(Dns, getHostEntry_address_failure)
{
    auto entry = net::Dns::getHostEntry(net::IpAddress::None);
    EXPECT_EQ(entry.address_list.size(), 0);
}
