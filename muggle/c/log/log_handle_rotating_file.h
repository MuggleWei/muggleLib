/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_C_LOG_HANDLE_ROTATING_FILE_H_
#define MUGGLE_C_LOG_HANDLE_ROTATING_FILE_H_

#include "muggle/c/base/macro.h"
#include "muggle/c/log/log_fmt.h"
#include "muggle/c/log/log_handle.h"

EXTERN_C_BEGIN

/*
 * initialize a rotating file log handle
 * @handle: rotating file log handle pointer
 * @write_type: use one of MUGGLE_LOG_WRITE_TYPE_*
 * @fmt_flag: use MUGGLE_LOG_FMT_*
 * @level: log level that the log handle will output
 * @async_capacity: if write_type == MUGGLE_LOG_WRITE_TYPE_ASYNC, use this specify async buffer capacity
 * @file_path: log file path
 * @max_bytes: max size when to rotate log
 * @backup_count: max backup file count
 * RETURN: success returns 0, otherwise return err code in err.h
 * */
MUGGLE_CC_EXPORT
int muggle_log_handle_rotating_file_init(
	muggle_log_handle_t *handle,
	int write_type,
	int fmt_flag,
	int level,
	muggle_atomic_int async_capacity,
	const char *file_path,
	unsigned int max_bytes,
	unsigned int backup_count);

/*
 * destroy a rotating file log handle
 * @handle: rotating file log handle pointer
 * RETURN: success returns 0, otherwise return err code in err.h
 * NOTE: don't invoke this function immediatly, use muggle_log_handle_destroy
 * */
MUGGLE_CC_EXPORT
int muggle_log_handle_rotating_file_destroy(muggle_log_handle_t *handle);

/*
 * output message
 * @handle: rotating file log handle pointer
 * @arg: log format arguments
 * @msg: log messages
 * RETURN: success return number of bytes be writed to output, otherwise return negative
 * NOTE: don't invoke this function immediatly, use muggle_log_handle_write
 * */
MUGGLE_CC_EXPORT
int muggle_log_handle_rotating_file_output(
	muggle_log_handle_t *handle,
	muggle_log_fmt_arg_t *arg,
	const char *msg
);

EXTERN_C_END

#endif