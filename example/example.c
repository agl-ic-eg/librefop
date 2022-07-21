/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	example.c
 * @brief	example for librefop
 */

#include "librefop.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

const char filedir[] = "/tmp/refop-test/";
const char filename[] = "refop-example.bin";

int main(int argc, char *argv[])
{
	refop_handle_t handle;
	refop_error_t ret = REFOP_SUCCESS;
	uint8_t *pbuf = NULL;
	int64_t sz = 128 * 1024;
	int64_t szr = 0;

	// Data directry need to make self.
	mkdir(filedir, 0777);

	// Create handle
	ret = refop_create_redundancy_handle(&handle, filedir, filename);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to create refop handle: return %d\n", (int) ret);
		return -1;
	}

	// Example data create
	pbuf = (uint8_t *) malloc(sz);
	if (pbuf == NULL) {
		fprintf(stderr, "Malloc fail\n");
		return -1;
	}
	memset(pbuf, 0xff, sz);

	// Set data
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to set data: return %d\n", (int) ret);
		return -1;
	}

	// Set data
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to set data: return %d\n", (int) ret);
		return -1;
	}

	// Set data
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to set data: return %d\n", (int) ret);
		return -1;
	}

	// Get data
	ret = refop_get_redundancy_data(handle, pbuf, sz, &szr);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to get data: return %d\n", (int) ret);
		return -1;
	}

	ret = refop_remove_redundancy_data(handle);
	if (ret != REFOP_SUCCESS) {
		fprintf(stderr, "Fail to remove data: return %d\n", (int) ret);
		return -1;
	}

	// Release handle
	(void) refop_release_redundancy_handle(handle);

	return 0;
}
