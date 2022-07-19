/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	fileop_test_unit.cpp
 * @brief	Unit test fot fileop.c
 */
#include <gtest/gtest.h>
#include "mock/syscall_io_mock.hpp"

// Test Terget files ---------------------------------------
extern "C" {
#include "../lib/fileop.c"
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct fileop_test_unit_test : Test, SyscallIOMockBase {};

//--------------------------------------------------------------------------------------------------------
//stubs
ssize_t g_safe_read_ret = 0;
ssize_t safe_read(int fd, void *buf, size_t count)
{
	return g_safe_read_ret;
}

ssize_t g_safe_write_ret = 0;
ssize_t safe_write(int fd, void *buf, size_t count)
{
	return g_safe_write_ret;
}

//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_new_file_write__arg_error)
{
	int ret = -1;
	refop_handle_t handle = NULL;
	uint8_t dmybuf[128];

	//dummy data

	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit() + 1);
	ASSERT_EQ(-2, ret);

	ret = refop_new_file_write(handle, dmybuf, 0);
	ASSERT_EQ(-2, ret);

	ret = refop_new_file_write(handle, dmybuf, -1);
	ASSERT_EQ(-2, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_new_file_write__unlink_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t dmybuf[128];

	EXPECT_CALL(sysiom, unlink(_)).WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(-1, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_new_file_write__open_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t *dmybuf = (uint8_t*)calloc(1, refop_get_config_data_size_limit());

	EXPECT_CALL(sysiom, unlink(_)).WillOnce(SetErrnoAndReturn(ENOENT, -1));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(-1));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, unlink(_)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(-1));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(-1, ret);

	free(dmybuf);
	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_new_file_write__safe_write_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));
	uint8_t *dmybuf = (uint8_t*)calloc(1, refop_get_config_data_size_limit());

	//dummy
	g_safe_read_ret = 0;
	g_safe_write_ret = 0;

	g_safe_write_ret = -1;
	EXPECT_CALL(sysiom, unlink(_)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(-1, ret);

	g_safe_write_ret = 0;
	EXPECT_CALL(sysiom, unlink(_)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(0, ret);

	g_safe_write_ret = 0;
	EXPECT_CALL(sysiom, unlink(_)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_new_file_write(handle, dmybuf, refop_get_config_data_size_limit());
	ASSERT_EQ(0, ret);

	free(dmybuf);
	free(handle);
}
#if 0
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_create_redundancy_handle__stat_error)
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
TEST_F(interface_test, interface_test_refop_create_redundancy_handle__pathcheck_error)
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
TEST_F(interface_test, interface_test_refop_create_redundancy_handle__success)
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
	free(handle);

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
	free(handle);
}
//--------------------------------------------------------------------------------------------------------
#endif
/*TEST_F(data_pool_test_others, test_data_pool_test_data_pool_unlock__false)
{
	bool ret = true;

	EXPECT_CALL(lpm, pthread_mutex_unlock(_))
			.WillOnce(Return(-1));
	ret = data_pool_unlock();
	ASSERT_EQ(false, ret);
}*/
