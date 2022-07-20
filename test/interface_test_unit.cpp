/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	interface_test_unit.cpp
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

struct interface_test_unit : Test, SyscallIOMockBase {};

//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit, interface_test_unit_refop_create_redundancy_handle__arg_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp";
	char file[] = "test.bin";

	ret = refop_create_redundancy_handle(NULL, NULL, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_create_redundancy_handle(&handle, NULL, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_create_redundancy_handle(NULL, directry, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_create_redundancy_handle(&handle, directry, NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_create_redundancy_handle(&handle, NULL, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_create_redundancy_handle(NULL, directry, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit, interface_test_unit_refop_create_redundancy_handle__stat_error)
{
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp";
	char file[] = "test.bin";

	/* stat error case
		EACCES
		EFAULT
		ELOOP
		ENAMETOOLONG
		ENOENT
		ENOMEM
		ENOTDIR
		EOVERFLOW
	*/

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_NOENT, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(EFAULT, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(ELOOP, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_NOENT, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(ENAMETOOLONG, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_NOENT, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(ENOMEM, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SYSERROR, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(ENOTDIR, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_NOENT, ret);

	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(SetErrnoAndReturn(EOVERFLOW, -1));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SYSERROR, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit, interface_test_unit_refop_create_redundancy_handle__pathcheck_error)
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
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	//short file string
	strncpy(directry,"/tmp",PATH_MAX);
	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(Return(0));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	// too long path
	for(int i=1;i < (PATH_MAX-1);i++)
		directry[i] = 'd';
	strncpy(file,"test.bin",PATH_MAX);
	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(Return(0));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_ARGERROR, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit, interface_test_unit_refop_create_redundancy_handle__success)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp";
	char directry2[] = "/tmp/";
	char file[] = "test.bin";
	char resultstr[] = "/tmp/test.bin";
	char resultstr_bk1[] = "/tmp/test.bin.bk1";
	char resultstr_new[] = "/tmp/test.bin.tmp";

	//short directry string
	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(Return(0));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	//data check
	hndl = (struct refop_halndle *)handle;
	ASSERT_EQ(0, strcmp(hndl->latestfile,resultstr));
	ASSERT_EQ(0, strcmp(hndl->backupfile1,resultstr_bk1));
	ASSERT_EQ(0, strcmp(hndl->newfile,resultstr_new));
	ASSERT_EQ(0, strcmp(hndl->basedir,directry2));
	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	//short file string
	EXPECT_CALL(sysiom, stat(directry2, _)).WillOnce(Return(0));
	ret = refop_create_redundancy_handle(&handle, directry2, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	//data check
	hndl = (struct refop_halndle *)handle;
	ASSERT_EQ(0, strcmp(hndl->latestfile,resultstr));
	ASSERT_EQ(0, strcmp(hndl->backupfile1,resultstr_bk1));
	ASSERT_EQ(0, strcmp(hndl->newfile,resultstr_new));
	ASSERT_EQ(0, strcmp(hndl->basedir,directry2));
	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test_unit, interface_test_unit_refop_release_redundancy_handle__all)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp";
	char directry2[] = "/tmp/";
	char file[] = "test.bin";
	char resultstr[] = "/tmp/test.bin";
	char resultstr_bk1[] = "/tmp/test.bin.bk1";
	char resultstr_new[] = "/tmp/test.bin.tmp";

	//short directry string
	EXPECT_CALL(sysiom, stat(directry, _)).WillOnce(Return(0));
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_release_redundancy_handle(NULL);
	ASSERT_EQ(REFOP_ARGERROR, ret);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);
}
//--------------------------------------------------------------------------------------------------------
