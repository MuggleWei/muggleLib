#include "socket_event_poll.h"
#include <stdlib.h>
#include <string.h>
#include "muggle/c/log/log.h"
#include "socket_event_utils.h"

#if MUGGLE_ENABLE_TRACE

static void muggle_ev_poll_debug_print(
        char *buf, int offset, int bufsize,
        muggle_socket_peer_t **p_peers, int cnt_fd)
{
    offset += snprintf(buf + offset, bufsize - offset, "current sockets: ");
	int cnt = 0;
    for (int i = 0; i < cnt_fd; i++)
    {
		if (cnt >= 5)
		{
			offset += snprintf(buf + offset, bufsize - offset, "... | ");
			break;
		}

		char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
		if (p_peers[i]->addr_len == 0 ||
			muggle_socket_ntop((struct sockaddr*)&p_peers[i]->addr, straddr, MUGGLE_SOCKET_ADDR_STRLEN, 0) == NULL)
		{
			snprintf(straddr, MUGGLE_SOCKET_ADDR_STRLEN, "?:?");
		}
#if MUGGLE_PLATFORM_WINDOWS
        offset += snprintf(buf + offset, bufsize - offset, "[%s] | ", straddr);
#else
        offset += snprintf(buf + offset, bufsize - offset, "%d[%s] | ", p_peers[i]->fd, straddr);
#endif

		++cnt;
    }

    offset += snprintf(buf + offset, bufsize - offset, "cnt_fd: %d", cnt_fd);
    MUGGLE_DEBUG_INFO(buf);
}

#endif

static int muggle_socket_event_poll_listen(
	muggle_socket_event_t *ev,
	muggle_socket_peer_t *listen_peer,
	struct pollfd *fds,
	muggle_socket_peer_t *peers,
	muggle_socket_peer_t **p_peers,
	int capacity, int *cnt_fd)
{
	while (1)
	{
		// get new connection
		muggle_socket_peer_t tmp_peer;
		muggle_socket_peer_t *peer;
		if (*cnt_fd == capacity)
		{
			peer = &tmp_peer;
		}
		else
		{
			peer = p_peers[*cnt_fd];
		}
		memset(peer, 0, sizeof(muggle_socket_peer_t));

		peer->addr_len = sizeof(peer->addr);
		peer->fd = accept(listen_peer->fd, (struct sockaddr*)&peer->addr, &peer->addr_len);
		if (peer->fd == MUGGLE_INVALID_SOCKET)
		{
			if (MUGGLE_SOCKET_LAST_ERRNO == MUGGLE_SYS_ERRNO_INTR)
			{
				continue;
			}
			else if (MUGGLE_SOCKET_LAST_ERRNO == MUGGLE_SYS_ERRNO_WOULDBLOCK)
			{
				return 0;
			}

			char err_msg[1024];
			muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
			MUGGLE_ERROR("failed accept - %s", err_msg);

			// close listen socket
			if (ev->on_error != NULL)
			{
				if (ev->on_error(ev, listen_peer) == 0)
				{
					muggle_socket_close(listen_peer->fd);
				}
			}
			else
			{
				muggle_socket_close(peer->fd);
			}

			return 1;
		}

		if (peer == &tmp_peer)
		{
			// close socket if number reached the upper limit
			char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
			if (muggle_socket_ntop((struct sockaddr*)&peer->addr, straddr, sizeof(straddr), 0) == NULL)
			{
				snprintf(straddr, sizeof(straddr), "unknown:unknown");
			}

			MUGGLE_WARNING("refuse connection %s - number of connection reached the upper limit", straddr);
			muggle_socket_close(peer->fd);
		}
		else
		{
			peer->peer_type = MUGGLE_SOCKET_PEER_TYPE_TCP_PEER;

			// set socket nonblock
			muggle_socket_set_nonblock(peer->fd, 1);

			int ret = 0;
			if (ev->on_connect)
			{
				ret = ev->on_connect(ev, listen_peer, peer);
			}

			if (ret == 0)
			{
				// add new connection socket peer
				fds[*cnt_fd].fd = peer->fd;
				fds[*cnt_fd].events = POLLIN;
				++(*cnt_fd);

#if MUGGLE_ENABLE_TRACE
				char debug_buf[4096];
				int debug_offset = 0;
				debug_offset = snprintf(debug_buf, sizeof(debug_buf) - debug_offset, "new connection |");
				muggle_ev_poll_debug_print(
					debug_buf, debug_offset, sizeof(debug_buf), p_peers, *cnt_fd);
#endif
			}
			else
			{
				if (ret == -1)
				{
					muggle_socket_close(peer->fd);
				}
			}
		}
	}
}

void muggle_socket_event_poll(muggle_socket_event_t *ev, muggle_socket_ev_arg_t *ev_arg)
{
	MUGGLE_TRACE("socket event poll run...");

	// set fd capacity
	int capacity = ev_arg->hints_max_peer;
	if (capacity <= 0)
	{
		capacity = 1024;
	}

	if (capacity < ev_arg->cnt_peer)
	{
		MUGGLE_WARNING("capacity space not enough for all peers");
		return;
	}

	// timer
	int timeout = ev_arg->timeout_ms;

	struct pollfd *fds = (struct pollfd*)malloc(capacity * sizeof(struct pollfd));
	muggle_socket_peer_t *peers = (muggle_socket_peer_t*)malloc(capacity * sizeof(muggle_socket_peer_t));
	muggle_socket_peer_t **p_peers = (muggle_socket_peer_t**)malloc(capacity * sizeof(muggle_socket_peer_t*));
	for (int i = 0; i < capacity; ++i)
	{
		p_peers[i] = &peers[i];
	}

	int cnt_fd = 0;
	for (int i = 0; i < ev_arg->cnt_peer; ++i)
	{
		fds[i].fd = ev_arg->peers[i].fd;
		fds[i].events = POLLIN;

		memcpy(&peers[i], &ev_arg->peers[i], sizeof(muggle_socket_peer_t));
		if (peers[i].peer_type == MUGGLE_SOCKET_PEER_TYPE_TCP_LISTEN ||
			peers[i].peer_type == MUGGLE_SOCKET_PEER_TYPE_TCP_PEER)
		{
			muggle_socket_set_nonblock(peers[i].fd, 1);
		}
		cnt_fd++;
	}

	while (1)
	{
#if MUGGLE_PLATFORM_WINDOWS
		int n = WSAPoll(fds, cnt_fd, timeout);
#else
		int n = poll(fds, cnt_fd, timeout);
#endif
		if (n > 0)
		{
			for (int i = cnt_fd - 1; i >= 0; --i)
			{
				int need_close = 0;
				if (fds[i].revents & POLLIN)
				{
					muggle_socket_peer_t *peer = p_peers[i];
					switch (peer->peer_type)
					{
					case MUGGLE_SOCKET_PEER_TYPE_TCP_LISTEN:
						{
							need_close = muggle_socket_event_poll_listen(ev, peer, fds, peers, p_peers, capacity, &cnt_fd);
						}break;
					case MUGGLE_SOCKET_PEER_TYPE_TCP_PEER:
					case MUGGLE_SOCKET_PEER_TYPE_UDP_PEER:
						{
							need_close = muggle_socket_event_peer_on_message(ev, peer);
						}break;
					default:
						{
							MUGGLE_ERROR("invalid peer type: %d", peer->peer_type);
						}break;
					}

					--n;
				}
				else if (fds[i].revents & (POLLIN | POLLERR))
				{
					if (ev->on_error != NULL)
					{
						if (ev->on_error(ev, p_peers[i]) == 0)
						{
							muggle_socket_close(p_peers[i]->fd);
						}
					}
					need_close = 1;
					--n;
				}

				if (need_close)
				{
					if (i != cnt_fd - 1)
					{
						muggle_socket_peer_t *p_tmp;
						p_tmp = p_peers[i];
						p_peers[i] = p_peers[cnt_fd - 1];
						p_peers[cnt_fd - 1] = p_tmp;

						memcpy(&fds[i], &fds[cnt_fd - 1], sizeof(struct pollfd));
					}
					--cnt_fd;

#if MUGGLE_ENABLE_TRACE
					char debug_buf[4096];
					int debug_offset = 0;
					debug_offset = snprintf(debug_buf, sizeof(debug_buf) - debug_offset, "disconnection |");
					muggle_ev_poll_debug_print(
						debug_buf, debug_offset, sizeof(debug_buf), p_peers, cnt_fd);
#endif
				}

				if (n <= 0)
				{
					break;
				}
			}
		}
		else if (n == 0)
		{
			ev->on_timer(ev);
		}
		else
		{
			char err_msg[1024];
			muggle_socket_strerror(MUGGLE_SOCKET_LAST_ERRNO, err_msg, sizeof(err_msg));
			MUGGLE_ERROR("failed select loop - %s", err_msg);

			for (int i = 0; i < cnt_fd; ++i)
			{
				muggle_socket_close(p_peers[i]->fd);
			}
			break;
		}
	}

	// free memory
	free(fds);
	free(peers);
	free(p_peers);
}