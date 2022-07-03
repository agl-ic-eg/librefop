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
#include "../lib/fileop.c"
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct fileop_test_utils : Test, SyscallIOMockBase {};
//struct data_pool_test_set_get_others : Test {};

//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_utils, fileop_test_utils_refop_header_create__success)
{
	s_refop_file_header header;
	s_refop_file_header *head = &header;

	refop_header_create(head, 0, 0);
	refop_header_create(head, 0x1234, 1*1024*1024*1024);
	refop_header_create(head, 0x5678, 64*1024*1024*1024ul);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_utils, fileop_test_utils_refop_header_validation__invalid_header)
{
	s_refop_file_header header;
	s_refop_file_header *head = &header;
	int ret = -1;

	// broaken magic
	refop_header_create(head, 0x1234, 64*1024*1024);
	head->magic = 0;
	ret = refop_header_validation(head);
	ASSERT_EQ(-1, ret);

	// broaken version
	refop_header_create(head, 0x1234, 64*1024*1024);
	head->version = 0x88888888;
	head->version_inv = 0x88888888;
	ret = refop_header_validation(head);
	ASSERT_EQ(-1, ret);

	// invalid version
	refop_header_create(head, 0x1234, 64*1024*1024);
	head->version = 0x88888888;
	head->version_inv = ~head->version;
	ret = refop_header_validation(head);
	ASSERT_EQ(-1, ret);

	// broaken crc
	refop_header_create(head, 0x1234, 64*1024*1024);
	head->crc16 = 0x8888;
	head->crc16_inv = 0x8888;
	ret = refop_header_validation(head);
	ASSERT_EQ(-1, ret);

	// broaken size
	refop_header_create(head, 0x1234, 64*1024*1024);
	head->size_inv = head->size;
	ret = refop_header_validation(head);
	ASSERT_EQ(-1, ret);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_utils, fileop_test_utils_refop_header_validation__valid_header)
{
	s_refop_file_header header;
	s_refop_file_header *head = &header;
	int ret = -1;

	// Valid header
	refop_header_create(head, 0x1234, 64*1024*1024);
	ret = refop_header_validation(head);
	ASSERT_EQ(0, ret);

	// Valid header
	refop_header_create(head, 0xabcd, 64*1024);
	ret = refop_header_validation(head);
	ASSERT_EQ(0, ret);

	// Valid header
	refop_header_create(head, 0x0000, 1*1024*1024);
	ret = refop_header_validation(head);
	ASSERT_EQ(0, ret);

}
//--------------------------------------------------------------------------------------------------------
TEST_F(fileop_test_utils, fileop_test_utils_refop_file_test__stat_error)
{
	int ret = -1;

	//dummy data
	char testfilename[] = "/tmp/test.bin";

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

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(ENOENT, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-1, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(EACCES, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(EFAULT, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(ELOOP, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(ENAMETOOLONG, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(ENOMEM, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(ENOTDIR, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(SetErrnoAndReturn(EOVERFLOW, -1));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(-2, ret);

	EXPECT_CALL(sysiom, stat(testfilename, _)).WillOnce(Return(0));
	ret = refop_file_test(testfilename);
	ASSERT_EQ(0, ret);
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
