#include "time_serv_handle.h"

int on_connect(
	struct muggle_socket_event *ev, struct muggle_socket_peer *listen_peer, struct muggle_socket_peer *peer)
{
	// output connection address string
	char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
	if (muggle_socket_ntop((struct sockaddr*)&peer->addr, straddr, MUGGLE_SOCKET_ADDR_STRLEN, 0) == NULL)
	{
		snprintf(straddr, MUGGLE_SOCKET_ADDR_STRLEN, "unknown:unknown");
	}
	MUGGLE_INFO("connect - %s", straddr);

	// save peer into container
	struct peer_container *container = (struct peer_container*)ev->datas;
	container->peers[container->cnt_peer] = peer;
	peer->data = (void*)(intptr_t)container->cnt_peer;
	++container->cnt_peer;

	return 0;
}

int on_error(struct muggle_socket_event *ev, struct muggle_socket_peer *peer)
{
	// output disconnection address string
	char straddr[MUGGLE_SOCKET_ADDR_STRLEN];
	if (muggle_socket_ntop((struct sockaddr*)&peer->addr, straddr, MUGGLE_SOCKET_ADDR_STRLEN, 0) == NULL)
	{
		snprintf(straddr, MUGGLE_SOCKET_ADDR_STRLEN, "unknown:unknown");
	}
	MUGGLE_INFO("disconnect - %s", straddr);

	// remove peer from container
	struct peer_container *container = (struct peer_container*)ev->datas;
	int idx = (int)(intptr_t)peer->data;
	if (idx != container->cnt_peer - 1)
	{
		container->peers[idx] = container->peers[container->cnt_peer - 1];
	}
	--container->cnt_peer;

	return 0;
}

void on_timer(struct muggle_socket_event *ev)
{
	// get timestamp
	char buf[65536];
	time_t ts = time(NULL);
	struct tm t;
	gmtime_r(&ts, &t);
	uint32_t n = (uint32_t)snprintf(&buf[4], sizeof(buf) - 4 - 1, "%d-%02d-%02d %02d:%02d:%02d",
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
		t.tm_hour, t.tm_min, t.tm_sec);
	buf[4 + n] = '\0';
	n += 1;
	*(uint32_t*)buf = htonl(n);

	struct peer_container *container = (struct peer_container*)ev->datas;

	// udp send
	if (container->udp_peer)
	{
		send(container->udp_peer->fd, buf, n + 4, 0);
	}

	// all tcp connection
	for (int i = 0; i < container->cnt_peer; ++i)
	{
		send(container->peers[i]->fd, buf, n + 4, 0);
	}
}
