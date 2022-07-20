/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	interface_test_unit_memory.cpp
 * @brief	Unit test fot libredundancyfileop.c
 */
#include <gtest/gtest.h>
#include "mock/syscall_io_mock.hpp"
#include "mock/memory_mock.hpp"

// Test Terget files ---------------------------------------
extern "C" {
#define malloc(X)	unittest_malloc(X)
#include "../lib/libredundancyfileop.c"
#undef malloc
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct interface_test_unit_memory_test : Test, SyscallIOMockBase, MemoryMockBase {};

//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit_memory_test, interface_test_unit_memory_test_refop_create_redundancy_handle__malloc_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[PATH_MAX];
	char file[PATH_MAX];

	memset(directry,0,sizeof(directry));
	memset(file,0,sizeof(file));

	//short directry string
	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(Return(0));
	EXPECT_CALL(memorym, malloc(_)).WillOnce(Return(NULL));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SYSERROR, ret);
}
