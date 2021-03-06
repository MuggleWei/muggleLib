/******************************************************************************
 *  @file         socket_event.c
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2021-06-17
 *  @copyright    Copyright 2021 Muggle Wei
 *  @license      MIT License
 *  @brief        socket event
 *****************************************************************************/
 
#include "socket_event.h"
#include <stdlib.h>
#include <string.h>
#include "muggle/c/log/log.h"
#include "event/socket_event_memmgr.h"
#include "event/socket_event_select.h"
#include "event/socket_event_poll.h"
#include "event/socket_event_epoll.h"

static int muggle_get_event_loop_type(int event_loop_type)
{
#if !MUGGLE_PLATFORM_LINUX
	if (event_loop_type == MUGGLE_SOCKET_EVENT_LOOP_TYPE_EPOLL)
	{
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_NULL;
	}
#endif

#if !MUGGLE_PLATFORM_WINDOWS
	if (event_loop_type == MUGGLE_SOCKET_EVENT_LOOP_TYPE_IOCP)
	{
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_NULL;
	}
#endif

#if !MUGGLE_PLATFORM_FREEBSD
	if (event_loop_type == MUGGLE_SOCKET_EVENT_LOOP_TYPE_KQUEUE)
	{
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_NULL;
	}
#endif

	if (event_loop_type <= MUGGLE_SOCKET_EVENT_LOOP_TYPE_NULL ||
		event_loop_type >= MUGGLE_SOCKET_EVENT_LOOP_TYPE_MAX)
	{
#if MUGGLE_PLATFORM_LINUX
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_EPOLL;
#elif MUGGLE_PLATFORM_WINDOWS
		// event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_IOCP;
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT;
#elif MUGGLE_PLATFORM_FREEBSD
		// event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_KQUEUE;
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT;
#else
		event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT;
#endif
	}

	return event_loop_type;
}

/*
 * init socket event input arguments
 * RETURN: 0 - success, otherwise failed
 * */
static int muggle_socket_ev_arg_init(muggle_socket_event_init_arg_t *ev_init_arg, muggle_socket_event_t *ev)
{
	memset(ev, 0, sizeof(muggle_socket_event_t));

	// set event loop type
	ev->ev_loop_type = muggle_get_event_loop_type(ev_init_arg->ev_loop_type);

	// set capacity
	int capacity = ev_init_arg->hints_max_peer;
	if (ev->ev_loop_type == MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT)
	{
		if (capacity <= 0 || capacity > FD_SETSIZE)
		{
			capacity = FD_SETSIZE;
		}
	}
	else
	{
		if (capacity <= 0)
		{
			capacity = 1024;
		}
	}
	if (capacity < ev_init_arg->cnt_peer)
	{
		capacity = ev_init_arg->cnt_peer * 2;
	}
	ev->capacity = capacity;

	// set timeout
	if (ev_init_arg->timeout_ms < 0)
	{
		ev->timeout_ms = -1;
	}
	else
	{
		ev->timeout_ms = ev_init_arg->timeout_ms;
	}

	ev->to_exit = 0;
	ev->datas = ev_init_arg->datas;

	// set callbacks
	ev->on_connect = ev_init_arg->on_connect;
	ev->on_error = ev_init_arg->on_error;
	ev->on_close = ev_init_arg->on_close;
	ev->on_message = ev_init_arg->on_message;
	ev->on_timer = ev_init_arg->on_timer;

	// init memory manager
	muggle_socket_event_memmgr_t *mem_mgr = (muggle_socket_event_memmgr_t*)malloc(sizeof(muggle_socket_event_memmgr_t));
	if (mem_mgr == NULL)
	{
		MUGGLE_LOG_ERROR("failed allocate space for socket event memory manager");
		return -1;
	}

	if (muggle_socket_event_memmgr_init(ev, ev_init_arg, mem_mgr) != 0)
	{
		MUGGLE_LOG_ERROR("failed init socket event memory manager");
		free(mem_mgr);
		return -1;
	}

	ev->mem_mgr = mem_mgr;

	return 0;
}

/*
 * event loop run
 * */
static int muggle_socket_event_loop_run(muggle_socket_event_t *ev)
{
	int ret = 0;
	switch (ev->ev_loop_type)
	{
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_MULTHREAD:
	{
		MUGGLE_LOG_ERROR("unimplemented event loop type: multhread, to be continued...");
		ret = -1;
	}break;
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT:
	{
		muggle_socket_event_select(ev);
	}break;
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_POLL:
	{
		muggle_socket_event_poll(ev);
	}break;
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_EPOLL:
	{
#if MUGGLE_PLATFORM_LINUX
		muggle_socket_event_epoll(ev);
#else
		MUGGLE_LOG_ERROR("epoll event loop support linux only");
		ret = -1;
#endif
	}break;
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_IOCP:
	{
#if MUGGLE_PLATFORM_WINDOWS
		MUGGLE_LOG_ERROR("unimplemented event loop type: iocp, to be continued...");
		ret = -1;
#else
		MUGGLE_LOG_ERROR("iocp event loop support windows only");
		ret = -1;
#endif
	}break;
	case MUGGLE_SOCKET_EVENT_LOOP_TYPE_KQUEUE:
	{
#if MUGGLE_PLATFORM_FREEBSD
		MUGGLE_LOG_ERROR("unimplemented event loop type: kqueue, to be continued...");
		ret = -1;
#else
		MUGGLE_LOG_ERROR("kqueue event loop support FreeBSD only");
		ret = -1;
#endif
	}break;
	default:
	{
		MUGGLE_LOG_ERROR("unsupport event loop type: %d", ev->ev_loop_type);
		ret = -1;
	}break;
	}

	return ret;
}

int muggle_socket_event_init(muggle_socket_event_init_arg_t *ev_init_arg, muggle_socket_event_t *ev)
{
	if (ev_init_arg == NULL || ev == NULL)
	{
		MUGGLE_LOG_ERROR("failed init socket event, input parameter is null");
		return -1;
	}

	// init socket event from init arguments
	int ret = muggle_socket_ev_arg_init(ev_init_arg, ev);
	if (ret != 0)
	{
		for (int i = 0; i < ev_init_arg->cnt_peer; ++i)
		{
			muggle_socket_peer_t *peer = &ev_init_arg->peers[i];
			if (peer)
			{
				muggle_socket_close(peer->fd);
			}
		}
		return ret;
	}

	return 0;
}

int muggle_socket_event_loop(muggle_socket_event_t *ev)
{
	if (ev == NULL || ev->mem_mgr == NULL)
	{
		MUGGLE_LOG_ERROR("failed socket event loop, input parameter is null");
		return -1;
	}

	int ret = muggle_socket_event_loop_run(ev);

	// destroy and free memory manager
	muggle_socket_event_memmgr_destroy((muggle_socket_event_memmgr_t*)ev->mem_mgr);
	free(ev->mem_mgr);
	ev->mem_mgr = NULL;

	return ret;
}

void muggle_socket_event_loop_exit(muggle_socket_event_t *ev)
{
	ev->to_exit = 1;
}
