#ifndef EXAMPLE_TIME_CLIENT_HANDLE_H_
#define EXAMPLE_TIME_CLIENT_HANDLE_H_

#include "muggle/c/muggle_c.h"

void on_error(struct muggle_socket_event *ev, struct muggle_socket_peer *peer);

void on_message(struct muggle_socket_event *ev, struct muggle_socket_peer *peer);

#endif