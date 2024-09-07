#pragma once

#ifdef _WIN32
    #ifdef NETCPP_BUILD_SHARED
        #define NETCPP_API __declspec(dllexport)
        #pragma warning(disable: 4251)
    #elif NETCPP_SHARED
        #define NETCPP_API __declspec(dllimport)
    #endif
#else
    #ifdef NETCPP_BUILD_SHARED
        #define NETCPP_API __attribute__((__visibility__("default")))
    #elif NETCPP_SHARED
        #define NETCPP_API __attribute__((__visibility__("default")))
    #endif
#endif
#ifndef NETCPP_API
    #define NETCPP_API
#endif