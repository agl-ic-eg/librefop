/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	fileop_test_set_get_remove.cpp
 * @brief	Unit test fot libredundancyfileop.c
 */
#include <gtest/gtest.h>
#include "mock/syscall_io_mock.hpp"

// Test Terget files ---------------------------------------
extern "C" {
#include "../lib/libredundancyfileop.c"
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct fileop_test_set_get_remove_test : Test, SyscallIOMockBase {};

//--------------------------------------------------------------------------------------------------------
// stubs
int g_refop_new_file_write_ret = 0;
int refop_new_file_write(refop_handle_t handle, uint8_t *data, int64_t bufsize)
{
	return g_refop_new_file_write_ret;
}

int g_refop_file_rotation_ret = 0;
int refop_file_rotation(refop_handle_t handle)
{
	return g_refop_file_rotation_ret;
}

int g_refop_file_pickup_ret = 0;
int refop_file_pickup(refop_handle_t handle, uint8_t *data, int64_t bufsize, int64_t *readsize)
{
	return g_refop_file_pickup_ret;
}

//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_set_redundancy_data__arg_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];

	//dummy data
	ret = refop_set_redundancy_data(NULL, NULL, -100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(handle, NULL, -100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(NULL, dmybuf, -100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(NULL, NULL, 100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(handle, dmybuf, -100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(handle, NULL, 100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_set_redundancy_data(NULL, dmybuf, 100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_set_redundancy_data__refop_new_file_write_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];

	//stub setup data
	g_refop_new_file_write_ret = 0;
	/*
	 * @retval 0 Succeeded.
	 * @retval -1 Abnormal fail. Shall not continue.
	 * @retval -2 Lager than size limit.
	*/
	g_refop_file_rotation_ret = 0;
	g_refop_file_pickup_ret = 0;

	g_refop_new_file_write_ret = -1;
	ret = refop_set_redundancy_data(handle, dmybuf, 100);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	g_refop_new_file_write_ret = -2;
	ret = refop_set_redundancy_data(handle, dmybuf, 100);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_set_redundancy_data__refop_file_rotation_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];

	//stub setup data
	g_refop_new_file_write_ret = 0;
	g_refop_file_rotation_ret = 0;
	/*
	 * @retval 0 Succeeded.
	 * @retval -1 Abnormal fail. Shall not continue.
	*/
	g_refop_file_pickup_ret = 0;

	g_refop_file_rotation_ret = -1;
	EXPECT_CALL(sysiom, unlink(_)).WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_set_redundancy_data(handle, dmybuf, 100);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	g_refop_file_rotation_ret = -1;
	EXPECT_CALL(sysiom, unlink(_)).WillOnce(Return(0));
	ret = refop_set_redundancy_data(handle, dmybuf, 100);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	g_refop_file_rotation_ret = 0;
	ret = refop_set_redundancy_data(handle, dmybuf, 100);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_get_redundancy_data__arg_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];
	int64_t getsize;

	//dummy data
	//all error
	ret = refop_get_redundancy_data(NULL, NULL, -100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	// 3 args error
	ret = refop_get_redundancy_data(handle, NULL, -100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, dmybuf, -100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, NULL, 100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, NULL, -100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	// 2 args error
	ret = refop_get_redundancy_data(handle, dmybuf, -100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(handle, NULL, 100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(handle, NULL, -100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, dmybuf, 100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, dmybuf, -100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, NULL, 100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	//1 arg error
	ret = refop_get_redundancy_data(handle, dmybuf, 100, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(handle, dmybuf, -100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(handle, NULL, 100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_get_redundancy_data(NULL, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_get_redundancy_data__refop_file_pickup_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];
	int64_t getsize;

	//stub setup data
	g_refop_new_file_write_ret = 0;
	g_refop_file_rotation_ret = 0;
	g_refop_file_pickup_ret = 0;
	/*
	 * @retval 0 Succeeded.
	 * @retval 1 Succeeded with recover.
	 * @retval -1 Abnormal fail. Shall not continue.
	 * @retval -2 No data.
	 * @retval -3 Broken data.
	 */
	g_refop_file_pickup_ret = -3;
	ret = refop_get_redundancy_data(handle, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_BROKEN, ret);

	g_refop_file_pickup_ret = -2;
	ret = refop_get_redundancy_data(handle, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_NOENT, ret);

	g_refop_file_pickup_ret = -1;
	ret = refop_get_redundancy_data(handle, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	g_refop_file_pickup_ret = 0;
	ret = refop_get_redundancy_data(handle, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	g_refop_file_pickup_ret = 1;
	ret = refop_get_redundancy_data(handle, dmybuf, 100, &getsize);
	ASSERT_EQ(REFOP_RECOVER, ret);

	free(handle);
}

//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_remove_redundancy_data__arg_error)
{
	refop_error_t ret = REFOP_SUCCESS;

	//dummy data
	ret = refop_remove_redundancy_data(NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_remove_redundancy_data__unlink_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));

	// 3 error
	// EACCES - EACCES - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// 2 error
	// success - EACCES - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// ENOENT - EACCES - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - success - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - ENOENT - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - EACCES - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - EACCES - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// 1 error
	// success - success - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// success - ENOENT - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// ENOENT - success - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// ENOENT - ENOENT - EACCES
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// success - EACCES - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// success - EACCES - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// ENOENT - EACCES - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// ENOENT - EACCES - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - success - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - success - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - ENOENT - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	// EACCES - ENOENT - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_set_get_remove_test, unit_test_refop_remove_redundancy_data__unlink_success)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));

	// success - success - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// ENOENT - ENOENT - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// ENOENT - success - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// success - ENOENT - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// success - success - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// ENOENT - ENOENT - success
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// success - ENOENT - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// ENOENT - success - ENOENT
	EXPECT_CALL(sysiom, unlink(_))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(handle);
}
