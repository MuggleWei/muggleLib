/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef BENCHMARK_CHANNEL_H_
#define BENCHMARK_CHANNEL_H_

#include "trans_runner.h"

int channel_write(void *trans_obj, void *data);
void* channel_read(void *trans_obj);
void run_channel(struct write_thread_args *args, muggle_benchmark_block_t *blocks, int num_thread, int msg_per_write, int read_busy_loop);

#endif
