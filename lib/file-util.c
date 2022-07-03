/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	file-util.c
 * @brief	EINTR safe file functions
 */
#include "file-util.h"

#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/**
 * INTR safe read
 * Interface spec is similar to read.
 */
ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t size = 0,ressize = 0;
	size_t reqsize = 0;
	uint8_t *pbuf = NULL;

	pbuf = (uint8_t*)buf;
	reqsize = count;

	do {
		size = read(fd, pbuf, (reqsize - ressize));
		if (size < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				ressize = size;
				break;
			}
		}

		pbuf += size;
		ressize += size;
	} while((ressize < reqsize) && (size != 0));

	return ressize;
}

/**
 * INTR safe write
 * Interface spec is similar to read.
 */
ssize_t safe_write(int fd, void *buf, size_t count)
{
	ssize_t size = 0,ressize = 0;
	size_t reqsize = 0;
	uint8_t *pbuf = NULL;

	pbuf = (uint8_t*)buf;
	reqsize = count;

	do {
		size = write(fd, pbuf, (reqsize - ressize));
		if (size < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				ressize = size;
				break;
			}
		}

		pbuf += size;
		ressize += size;
	} while((ressize < reqsize) && (size != 0));

	return ressize;
}
