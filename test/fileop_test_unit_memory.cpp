/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	fileop_test_unit_memory.cpp
 * @brief	Unit test fot fileop.c
 */
#include <gtest/gtest.h>
#include "mock/syscall_io_mock.hpp"
#include "mock/memory_mock.hpp"

// Test Terget files ---------------------------------------
extern "C" {
#define malloc(X)	unittest_malloc(X)
#include "../lib/fileop.c"
#undef malloc
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct fileop_test_unit_memory_test : Test, SyscallIOMockBase, MemoryMockBase {};

//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_memory_test, unit_test_refop_new_file_write__open_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t *dmybuf = (uint8_t*)calloc(1, refop_get_config_data_size_limit());

	EXPECT_CALL(sysiom, unlink(_)).WillOnce(SetErrnoAndReturn(ENOENT, -1));
	EXPECT_CALL(memorym, malloc(_)).WillOnce(Return(NULL));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(-1, ret);

	free(dmybuf);
	free(handle);
}
