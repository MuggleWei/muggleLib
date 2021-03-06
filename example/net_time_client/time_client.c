#include "time_client_handle.h"

int main(int argc, char *argv[])
{
	// initialize log
	if (muggle_log_simple_init(MUGGLE_LOG_LEVEL_INFO, MUGGLE_LOG_LEVEL_INFO) != 0)
	{
		MUGGLE_LOG_ERROR("failed init log");
		exit(EXIT_FAILURE);
	}

	if (argc != 4)
	{
		MUGGLE_LOG_ERROR("usage: %s <host> <port> <tcp|udp|mcast>", argv[0]);
		exit(EXIT_FAILURE);
	}

	// initialize socket library
	muggle_socket_lib_init();

	// reconnect 3 times
	int tcp_contiguous_failed = 0;
	while (1)
	{
		// create peer
		muggle_socket_peer_t peer;
		if (strcmp(argv[3], "tcp") == 0)
		{
			muggle_tcp_connect(argv[1], argv[2], 3, &peer);
		}
		else if (strcmp(argv[3], "udp") == 0)
		{
			muggle_udp_bind(argv[1], argv[2], &peer);
		}
		else if (strcmp(argv[3], "mcast") == 0)
		{
			muggle_mcast_join(argv[1], argv[2], NULL, NULL, &peer);
		}
		else
		{
			MUGGLE_LOG_ERROR("invalid socket peer type: %s", argv[3]);
			exit(EXIT_FAILURE);
		}

		if (peer.fd == MUGGLE_INVALID_SOCKET)
		{
			if (strcmp(argv[3], "tcp") == 0)
			{
				MUGGLE_LOG_ERROR("failed connect %s:%s", argv[1], argv[2]);
				if (++tcp_contiguous_failed >= 3)
				{
					exit(EXIT_FAILURE);
				}
				else
				{
					muggle_msleep(3000);
					MUGGLE_LOG_INFO("try to reconnect %s:%s", argv[1], argv[2]);
				}
				continue;
			}
			else
			{
				MUGGLE_LOG_ERROR("%s failed create socket for: %s:%s", argv[3], argv[1], argv[2]);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(argv[3], "tcp") == 0)
		{
			MUGGLE_LOG_INFO("%s success connect %s:%s", argv[3], argv[1], argv[2]);
		}

		// reset tcp contiguous connection failed count
		tcp_contiguous_failed = 0;

		// create bytes buffer for socket
		muggle_bytes_buffer_t bytes_buf;
		muggle_bytes_buffer_init(&bytes_buf, 16 * 1024 * 1024);
		peer.data = &bytes_buf;

		// fill up event loop input arguments
#if MUGGLE_PLATFORM_LINUX
		int event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_EPOLL;
#else
		int event_loop_type = MUGGLE_SOCKET_EVENT_LOOP_TYPE_SELECT;
#endif

		// fill up event loop input arguments
		muggle_socket_event_init_arg_t ev_init_arg;
		memset(&ev_init_arg, 0, sizeof(ev_init_arg));
		ev_init_arg.ev_loop_type = event_loop_type;
		ev_init_arg.hints_max_peer = 1024;
		ev_init_arg.cnt_peer = 1;
		ev_init_arg.peers = &peer;
		ev_init_arg.timeout_ms = -1;
		ev_init_arg.datas = NULL;
		ev_init_arg.on_connect = NULL;
		ev_init_arg.on_error = on_error;
		ev_init_arg.on_message = on_message;
		ev_init_arg.on_timer = NULL;

		// event loop
		muggle_socket_event_t ev;
		if (muggle_socket_event_init(&ev_init_arg, &ev) != 0)
		{
			MUGGLE_LOG_ERROR("failed init socket event");
			exit(EXIT_FAILURE);
		}
		muggle_socket_event_loop(&ev);

		// free bytes buffer
		muggle_bytes_buffer_destroy(&bytes_buf);
	}

	return 0;
}