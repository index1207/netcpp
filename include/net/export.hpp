#pragma once

#ifndef NETCPP_STATIC
    #ifdef _WIN32
        #define NETCPP_API_EXPORT __declspec(dllexport)
        #define NETCPP_API_IMPORT __declspec(dllimport)
        #pragma warning(disable: 4251)
    #else
        #define NETCPP_API_EXPORT __attribute__((__visibility__("default")))
        #define NETCPP_API_IMPORT __attribute__((__visibility__("default")))
    #endif
#else
    #define NETCPP_API_EXPORT
    #define NETCPP_API_IMPORT
#endif

#ifdef NETCPP_BUILD_SHARED
#define NETCPP_API NETCPP_API_EXPORT
#else
#define NETCPP_API NETCPP_API_IMPORT
#endif
