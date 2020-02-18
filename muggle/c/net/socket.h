/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_C_SOCKET_H_
#define MUGGLE_C_SOCKET_H_

#include "muggle/c/base/macro.h"

EXTERN_C_BEGIN

#if MUGGLE_PLATFORM_WINDOWS

// NOTE: if without WIN32_LEAN_AND_MEAN defined, must
// include winsock2.h before windows.h
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define MUGGLE_INVALID_SOCKET INVALID_SOCKET
#define MUGGLE_SOCKET_ERROR SOCKET_ERROR
#define MUGGLE_SOCKET_LAST_ERRNO WSAGetLastError()
#define MUGGLE_SOCKET_ADDR_STRLEN (INET6_ADDRSTRLEN + 8)

#define MUGGLE_SOCKET_SHUT_RD     SD_RECEIVE
#define MUGGLE_SOCKET_SHUT_WR     SD_SEND
#define MUGGLE_SOCKET_SHUT_RDWR   SD_BOTH

#define MUGGLE_SYS_ERRNO_INTR       WSAEINTR
#define MUGGLE_SYS_ERRNO_WOULDBLOCK WSAEWOULDBLOCK

typedef SOCKET muggle_socket_t;
typedef int muggle_socklen_t;

#else // MUGGLE_PLATFORM_WINDOWS

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>

#define MUGGLE_INVALID_SOCKET     (-1)
#define MUGGLE_SOCKET_ERROR       (-1)
#define MUGGLE_SOCKET_LAST_ERRNO  errno
#define MUGGLE_SOCKET_ADDR_STRLEN (INET6_ADDRSTRLEN + 8)

#define MUGGLE_SOCKET_SHUT_RD     SHUT_RD
#define MUGGLE_SOCKET_SHUT_WR     SHUT_WR
#define MUGGLE_SOCKET_SHUT_RDWR   SHUT_RDWR

#define MUGGLE_SYS_ERRNO_INTR       EINTR
#define MUGGLE_SYS_ERRNO_WOULDBLOCK EWOULDBLOCK

typedef int muggle_socket_t;
typedef socklen_t muggle_socklen_t;

#endif // MUGGLE_PLATFORM_WINDOWS

/*
 * initialize socket library
 * RETURN: return 0 if success, otherwise failed
 * */
MUGGLE_CC_EXPORT
int muggle_socket_lib_init();

/*
 * create socket
 * @arguments: the same as socket function
 * RETURN: on success, a socket descriptor is returned 
 *         on error, MUGGLE_INVALID_SOCKET is returned, and MUGGLE_SOCKET_LAST_ERRNO is set
 * */
MUGGLE_CC_EXPORT
muggle_socket_t muggle_socket_create(int family, int type, int protocol);

/*
 * close socket
 * RETURN: returns 0 on success, on error, -1 is returned and MUGGLE_SOCKET_LAST_ERRNO is set
 * */
MUGGLE_CC_EXPORT
int muggle_socket_close(muggle_socket_t fd);

/*
 * shutdown socket
 * RETURN: returns 0 on success, on error, -1 is returned and MUGGLE_SOCKET_LAST_ERRNO is set
 * */
MUGGLE_CC_EXPORT
int muggle_socket_shutdown(muggle_socket_t fd, int how);

/*
 * return string describing MUGGLE_SOCKET_LAST_ERRNO
 * @errnum: get from MUGGLE_SOCKET_LAST_ERRNO
 * @buf: buffer that stores error string
 * @bufsize: size of buffer
 * RETURN: returns 0 on success
 * */
MUGGLE_CC_EXPORT
int muggle_socket_strerror(int errnum, char *buf, size_t bufsize);

/*
 * set socket non block
 * @on: if 0, set block, otherwise set non block
 * RETURN: returns 0 on success, on error, -1 is returned and MUGGLE_SOCKET_LAST_ERRNO is set
 * */
MUGGLE_CC_EXPORT
int muggle_socket_set_nonblock(muggle_socket_t socket, int on);

EXTERN_C_END

#endif
