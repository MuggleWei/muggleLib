/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_MACRO_H_
#define MUGGLE_MACRO_H_

// debug and release macro
#if ! defined(NDEBUG)
	#define MUGGLE_DEBUG 1
#else
	#define MUGGLE_RELEASE 1
#endif

// build with trace info
#if MUGGLE_DEBUG & defined(MUGGLE_BUILD_TRACE)
	#define MUGGLE_ENABLE_TRACE 1
#endif

// detect platform
#ifdef _WIN32
	#define MUGGLE_PLATFORM_WINDOWS 1
	#ifdef _WIN64
		#define MUGGLE_PLATFORM_WIN64 1
	#endif
#elif __APPLE__
	#define MUGGLE_PLATFORM_APPLE 1
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		// iOS Simulator
	#elif TARGET_OS_IPHONE
		// iOS device
	#elif TARGET_OS_MAC
		// Other kinds of Mac OS
	#else
		// Unknown Apple platform
	#endif
#elif __linux__
	#define MUGGLE_PLATFORM_LINUX 1
#elif __FreeBSD__
	#define MUGGLE_PLATFORM_FREEBSD 1
#elif __unix__
	#define MUGGLE_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
	// POSIX
#else
	// Unknown compiler
#endif

// extern c
#ifdef __cplusplus
	#define EXTERN_C_BEGIN extern "C" {
	#define EXTERN_C_END   }
#else
	#define EXTERN_C_BEGIN
	#define EXTERN_C_END
#endif

// namespace
#ifdef __cplusplus
	#define NS_MUGGLE_BEGIN namespace muggle {
	#define NS_MUGGLE_END   }
	#define USING_NS_MUGGLE using namespace muggle
#else
	#define NS_MUGGLE_BEGIN
	#define NS_MUGGLE_END  
	#define USING_NS_MUGGLE
#endif 

// lib and dll
#if MUGGLE_PLATFORM_WINDOWS && defined(MUGGLE_CC_USE_DLL)
	#ifdef MUGGLE_CC_EXPORTS
		#define MUGGLE_CC_EXPORT __declspec(dllexport)
	#else
		#define MUGGLE_CC_EXPORT __declspec(dllimport)
	#endif
#else
	#define MUGGLE_CC_EXPORT
#endif

// deprecated macro
// #define MUGGLE_DEPRECATED [[deprecated]] // need c++14 support
#if MUGGLE_PLATFORM_WINDOWS
	#define MUGGLE_DEPRECATED __declspec(deprecated)
#elif MUGGLE_PLATFORM_UNIX || MUGGLE_PLATFORM_OSX
	#define MUGGLE_DEPRECATED __attribute__((deprecated))
#else
	#define MUGGLE_DEPRECATED
#endif

// max length of path
#if MUGGLE_PLATFORM_WINDOWS
	#define MUGGLE_MAX_PATH 260
#else
	#define MUGGLE_MAX_PATH 1024
#endif

// snprintf
#if MUGGLE_PLATFORM_WINDOWS
#if _MSC_VER <= 1800 // VS 2013
	#define snprintf(buf, size_in_byte, format, ...) sprintf_s(buf, size_in_byte, format, ##__VA_ARGS__)
#endif
#endif

// wait ifinite macro for mutex,cond,semaphore etc
#if MUGGLE_PLATFORM_WINDOWS
#define MUGGLE_WAIT_INFINITE INFINITE
#else
#define MUGGLE_WAIT_INFINITE ULONG_MAX
#endif

// fileno
#if MUGGLE_PLATFORM_WINDOWS
#define MUGGLE_FILENO _fileno
#else
#define MUGGLE_FILENO fileno
#endif

// winsock
#if MUGGLE_PLATFORM_WINDOWS

///////////////////////////////////////////////////////////////////
// From Windows NOTE: 
// https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application
// The Iphlpapi.h header file is required if an application is 
// using the IP Helper APIs.When the Iphlpapi.h header file is 
// required, the #include line for the Winsock2.h header this 
// file should be placed before the #include line for the 
// Iphlpapi.h header file.
// 
// The Winsock2.h header file internally includes core elements 
// from the Windows.h header file, so there is not usually 
// an #include line for the Windows.h header file in Winsock 
// applications.If an #include line is needed for the Windows.h 
// header file, this should be preceded with 
// the #define WIN32_LEAN_AND_MEAN macro.For historical reasons,
// the Windows.h header defaults to including the Winsock.h header 
// file for Windows Sockets 1.1.The declarations in the Winsock.h 
// header file will conflict with the declarations in the Winsock2.h
// header file required by Windows Sockets 2.0.The WIN32_LEAN_AND_MEAN
// macro prevents the Winsock.h from being included by the Windows.h 
// header.An example illustrating this is shown below.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#endif

// cache line padding in structure
#define MUGGLE_CACHE_LINE_SIZE 64
#define MUGGLE_STRUCT_CACHE_LINE_PADDING(idx) char cache_line_padding_##idx[MUGGLE_CACHE_LINE_SIZE]

// idx % capacity, capacity must be pow of 2
#define IDX_IN_POW_OF_2_RING(idx, capacity) ((idx) & ((capacity) - 1))

#endif
