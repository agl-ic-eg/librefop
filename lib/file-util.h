/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	file-util.h
 * @brief	file utility functions
 */
#ifndef REFOP_FILE_UTIL_H
#define REFOP_FILE_UTIL_H
//-----------------------------------------------------------------------------
#include <unistd.h>

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------

ssize_t safe_read(int fd, void *buf, size_t count);
ssize_t safe_write(int fd, void *buf, size_t count);

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif //#ifndef REFOP_FILE_UTIL_H
