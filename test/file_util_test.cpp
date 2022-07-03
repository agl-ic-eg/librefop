/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	data-pool-service-test.c
 * @brief	Unit test fot data-pool-service-test.c
 */
#include <gtest/gtest.h>
#include "mock/syscall_io_mock.hpp"

// Test Terget files ---------------------------------------
extern "C" {
#include "../lib/file-util.c"
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct file_util_test : Test, SyscallIOMockBase {};

//--------------------------------------------------------------------------------------------------------
TEST_F(file_util_test, file_util_test_safe_read__success)
{
	ssize_t ret = -1;
	uint8_t buffer[1024*1024];
	size_t sz = 1024*1024;
	size_t sza = sz - 1024;
	size_t szb = 1024;

	EXPECT_CALL(sysiom, read(1,buffer,sz)).WillOnce(SetErrnoAndReturn(EIO, -1));
	ret = safe_read(1, buffer, sz);
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, read(1,buffer,sz)).WillOnce(Return(sz));
	ret = safe_read(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, read(1,buffer,sz))
		.WillOnce(SetErrnoAndReturn(EINTR, -1))
		.WillOnce(Return(sz));
	ret = safe_read(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, read(1,buffer,sz)).WillOnce(Return(sza));
	EXPECT_CALL(sysiom, read(1,(&buffer[sza]),szb)).WillOnce(Return(szb));
	ret = safe_read(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, read(1,buffer,sz)).WillOnce(Return(sza));
	EXPECT_CALL(sysiom, read(1,(&buffer[sza]),szb)).WillOnce(Return(0));
	ret = safe_read(1, buffer, sz);
	ASSERT_EQ(sza, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(file_util_test, file_util_test_safe_write__success)
{
	ssize_t ret = -1;
	uint8_t buffer[1024*1024];
	size_t sz = 1024*1024;
	size_t sza = sz - 1024;
	size_t szb = 1024;

	EXPECT_CALL(sysiom, write(1,buffer,sz)).WillOnce(SetErrnoAndReturn(EIO, -1));
	ret = safe_write(1, buffer, sz);
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, write(1,buffer,sz)).WillOnce(Return(sz));
	ret = safe_write(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, write(1,buffer,sz))
		.WillOnce(SetErrnoAndReturn(EINTR, -1))
		.WillOnce(Return(sz));
	ret = safe_write(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, write(1,buffer,sz)).WillOnce(Return(sza));
	EXPECT_CALL(sysiom, write(1,(&buffer[sza]),szb)).WillOnce(Return(szb));
	ret = safe_write(1, buffer, sz);
	ASSERT_EQ(sz, ret);

	EXPECT_CALL(sysiom, write(1,buffer,sz)).WillOnce(Return(sza));
	EXPECT_CALL(sysiom, write(1,(&buffer[sza]),szb)).WillOnce(Return(0));
	ret = safe_write(1, buffer, sz);
	ASSERT_EQ(sza, ret);
}
