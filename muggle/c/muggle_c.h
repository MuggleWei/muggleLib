/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_C_H_
#define MUGGLE_C_H_

// base c header
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>

// muggle c base header
#include "muggle/c/base/macro.h"
#include "muggle/c/base/err.h"
#include "muggle/c/base/utils.h"
#include "muggle/c/base/str.h"
#include "muggle/c/base/thread.h"
#include "muggle/c/base/atomic.h"
#include "muggle/c/base/sleep.h"

// memory
#include "muggle/c/memory/memory_pool.h"
#include "muggle/c/memory/sowr_memory_pool.h"
#include "muggle/c/memory/memory_detect.h"
#include "muggle/c/memory/bytes_buffer.h"
#include "muggle/c/memory/threadsafe_memory_pool.h"
#include "muggle/c/memory/pointer_slot.h"

// time
#include "muggle/c/time/win_gettimeofday.h"
#include "muggle/c/time/win_gmtime.h"
#include "muggle/c/time/cpu_cycle.h"

// os
#include "muggle/c/os/os.h"
#include "muggle/c/os/path.h"
#include "muggle/c/os/dl.h"
#include "muggle/c/os/stacktrace.h"

// sync
#include "muggle/c/sync/mutex.h"
#include "muggle/c/sync/condition_variable.h"
#include "muggle/c/sync/futex.h"
#include "muggle/c/sync/ring_buffer.h"
#include "muggle/c/sync/array_blocking_queue.h"
#include "muggle/c/sync/double_buffer.h"
#include "muggle/c/sync/channel.h"

// log
#include "muggle/c/log/log_fmt.h"
#include "muggle/c/log/log_handle.h"
#include "muggle/c/log/log_handle_console.h"
#include "muggle/c/log/log_handle_file.h"
#include "muggle/c/log/log_handle_rotating_file.h"
#include "muggle/c/log/log_handle_win_debug.h"
#include "muggle/c/log/log_category.h"
#include "muggle/c/log/log.h"
#include "muggle/c/log/log_utils.h"

// net
#include "muggle/c/net/socket.h"
#include "muggle/c/net/socket_peer.h"
#include "muggle/c/net/socket_utils.h"
#include "muggle/c/net/socket_event.h"

// crypt
#include "muggle/c/crypt/crypt_utils.h"
#include "muggle/c/crypt/parity.h"
#include "muggle/c/crypt/des.h"
#include "muggle/c/crypt/tdes.h"
#include "muggle/c/crypt/aes.h"

// version
#include "muggle/c/version/version.h"

// data structure and algorithm
#include "muggle/c/dsaa/array_list.h"
#include "muggle/c/dsaa/linked_list.h"
#include "muggle/c/dsaa/stack.h"
#include "muggle/c/dsaa/queue.h"
#include "muggle/c/dsaa/trie.h"
#include "muggle/c/dsaa/avl_tree.h"
#include "muggle/c/dsaa/hash_table.h"
#include "muggle/c/dsaa/heap.h"
#include "muggle/c/dsaa/sort.h"

#endif
