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
	if (g_safe_read_ret == sizeof(s_refop_file_header)) {
		refop_header_create((s_refop_file_header*)buf, 100, refop_get_config_data_size_limit()+1);
	}
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
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_file_rotation__stat_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));

	//dummy
	g_safe_read_ret = 0;
	g_safe_write_ret = 0;
	g_safe_write_ret = 0;
	
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(EACCES, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(-1, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_file_rotation__open_error)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));

	//dummy
	g_safe_read_ret = 0;
	g_safe_write_ret = 0;
	g_safe_write_ret = 0;

	// use a4 mode
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	EXPECT_CALL(sysiom, rename(_, _))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_, _))
		.WillOnce(Return(-1));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(0, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, unit_test_refop_file_rotation__a1_a2_a3_a4)
{
	int ret = -1;
	refop_handle_t handle = (refop_handle_t)calloc(1,sizeof(struct refop_halndle));

	//dummy
	g_safe_read_ret = 0;
	g_safe_write_ret = 0;
	g_safe_write_ret = 0;
	strncpy(handle->backupfile1,"backup1",sizeof(handle->backupfile1));
	strncpy(handle->latestfile,"latestfile",sizeof(handle->backupfile1));
	strncpy(handle->newfile,"newfile",sizeof(handle->backupfile1));

	// a1 mode
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(Return(0))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, unlink(handle->backupfile1))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->latestfile, handle->backupfile1))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->newfile, handle->latestfile))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(0, ret);

	// a2 mode
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(Return(0))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	//EXPECT_CALL(sysiom, unlink(handle->backupfile1))
	//	.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->latestfile, handle->backupfile1))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->newfile, handle->latestfile))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(0, ret);

	// a3 mode
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(Return(0));
	//EXPECT_CALL(sysiom, unlink(handle->backupfile1))
	//	.WillOnce(Return(0));
	//EXPECT_CALL(sysiom, rename(handle->latestfile, handle->backupfile1))
	//	.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->newfile, handle->latestfile))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(0, ret);

	// a4 mode
	EXPECT_CALL(sysiom, stat(_, _))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1))
		.WillOnce(SetErrnoAndReturn(ENOENT, -1));
	//EXPECT_CALL(sysiom, unlink(handle->backupfile1))
	//	.WillOnce(Return(0));
	//EXPECT_CALL(sysiom, rename(handle->latestfile, handle->backupfile1))
	//	.WillOnce(Return(0));
	EXPECT_CALL(sysiom, rename(handle->newfile, handle->latestfile))
		.WillOnce(Return(0));
	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	EXPECT_CALL(sysiom, fsync(100)).WillOnce(Return(0));
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	ret = refop_file_rotation(handle);
	ASSERT_EQ(0, ret);

	free(handle);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, fileop_test_unit_test_refop_file_get_with_validation__1st_open_error)
{
	int ret = -1;
	
	//dummy data
	char testfilename[] = "/tmp/test.bin";
	uint8_t *pbuf = NULL;
	int64_t sz = 256 * 1024;
	int64_t szr = 0;

	pbuf = (uint8_t*)malloc(sz);

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-6, ret);

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(SetErrnoAndReturn(ENOMEM, -1));
	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-6, ret);

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-1, ret);

	free(pbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, fileop_test_unit_test_refop_file_get_with_validation__safe_read_error)
{
	int ret = -1;
	
	//dummy data
	char testfilename[] = "/tmp/test.bin";
	uint8_t *pbuf = NULL;
	int64_t sz = 256 * 1024;
	int64_t szr = 0;

	pbuf = (uint8_t*)malloc(sz);

	g_safe_read_ret = 0;
	g_safe_write_ret = 0;

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	g_safe_read_ret = sizeof(s_refop_file_header)*2;
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));
	
	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(200));
	g_safe_read_ret = sizeof(s_refop_file_header)/2;
	EXPECT_CALL(sysiom, close(200)).WillOnce(Return(0));

	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-2, ret);

	free(pbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_unit_test, fileop_test_unit_test_refop_file_get_with_validation__header_error)
{
	int ret = -1;
	
	//dummy data
	char testfilename[] = "/tmp/test.bin";
	uint8_t *pbuf = NULL;
	int64_t sz = 256 * 1024;
	int64_t szr = 0;

	pbuf = (uint8_t*)malloc(sz);

	g_safe_read_ret = 0;
	g_safe_write_ret = 0;

	EXPECT_CALL(sysiom, open(_,_)).WillOnce(Return(100));
	g_safe_read_ret = sizeof(s_refop_file_header);
	EXPECT_CALL(sysiom, close(100)).WillOnce(Return(0));

	ret = refop_file_get_with_validation(testfilename, pbuf, sz, &szr);
	ASSERT_EQ(-4, ret);

	free(pbuf);
}

