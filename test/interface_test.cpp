/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	interface_test.cpp
 * @brief	Public interface test fot refop
 */
#include <gtest/gtest.h>

// Test Terget files ---------------------------------------
extern "C" {
#include "../lib/libredundancyfileop.c"
}
// Test Terget files ---------------------------------------
using namespace ::testing;

struct interface_test : Test {};

//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_set_redundancy_data__success)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp/refop-test/";
	char file[] = "test.bin";
	uint8_t *pbuf = NULL;
	int64_t sz = 1 * 1024 * 1024;

	(void)mkdir(directry, 0777);

	pbuf = (uint8_t*)malloc(sz);

	//short directry string
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0xff,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0xa5,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0x5a,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(pbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_get_redundancy_data__success)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp/refop-test/";
	char file[] = "test.bin";
	uint8_t *pbuf = NULL;
	int64_t sz = 1 * 1024 * 1024;
	int64_t szr = 0;

	(void)mkdir(directry, 0777);

	pbuf = (uint8_t*)malloc(sz);

	//short directry string
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_get_redundancy_data(handle, pbuf, sz, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz, szr);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(pbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_set_and_get)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp/refop-test/";
	char file[] = "test.bin";
	char newfile[] = "/tmp/refop-test/test.bin.tmp";
	char latestfile[] = "/tmp/refop-test/test.bin";
	char backupfile[] = "/tmp/refop-test/test.bin.bk1";

	uint8_t *pbuf = NULL;
	uint8_t *prbuf = NULL;
	int64_t sz = 64 * 1024;
	int64_t szr = 0;
	int checker = 0;

	(void)mkdir(directry, 0777);

	pbuf = (uint8_t*)malloc(sz);
	prbuf = (uint8_t*)malloc(sz);

	//short directry string
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0xff,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// Operation algorithm
	//     Current                 Next
	//    | latest | backup |     | latest | backup |
	// a1 |    1   |   2    |     |  new   |    1   |
	// a2 |    1   |   x    |     |  new   |    1   |
	// a3 |    x   |   2    |     |  new   |    2   |
	// a4 |    x   |   x    |     |  new   |    x   |

	// a1
	ret = refop_get_redundancy_data(handle, prbuf, sz, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xff);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa1,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);


	// a2
	unlink(backupfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa2,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a3
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz, &szr);
	ASSERT_EQ(REFOP_RECOVER, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa3,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a4
	unlink(backupfile);
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz, &szr);
	ASSERT_EQ(REFOP_NOENT, ret);

	memset(pbuf,0xa4,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(pbuf);
	free(prbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_set_and_get_smallread)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp/refop-test/";
	char file[] = "test.bin";
	char newfile[] = "/tmp/refop-test/test.bin.tmp";
	char latestfile[] = "/tmp/refop-test/test.bin";
	char backupfile[] = "/tmp/refop-test/test.bin.bk1";

	uint8_t *pbuf = NULL;
	uint8_t *prbuf = NULL;
	int64_t sz = 4 * 1024;
	int64_t szr = 0;
	int checker = 0;

	(void)mkdir(directry, 0777);

	pbuf = (uint8_t*)malloc(sz);
	prbuf = (uint8_t*)malloc(sz);

	//short directry string
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0xff,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// Operation algorithm
	//     Current                 Next
	//    | latest | backup |     | latest | backup |
	// a1 |    1   |   2    |     |  new   |    1   |
	// a2 |    1   |   x    |     |  new   |    1   |
	// a3 |    x   |   2    |     |  new   |    2   |
	// a4 |    x   |   x    |     |  new   |    x   |

	// a1
	ret = refop_get_redundancy_data(handle, prbuf, sz/2, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz/2, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xff);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa1,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);


	// a2
	unlink(backupfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz/2, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz/2, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa2,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a3
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz/2, &szr);
	ASSERT_EQ(REFOP_RECOVER, ret);
	ASSERT_EQ(sz/2, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa3,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a4
	unlink(backupfile);
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz/2, &szr);
	ASSERT_EQ(REFOP_NOENT, ret);

	memset(pbuf,0xa4,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(pbuf);
	free(prbuf);
}
//--------------------------------------------------------------------------------------------------------
TEST_F(interface_test, interface_test_refop_set_and_get_largeread)
{
	struct refop_halndle *hndl;
	refop_error_t ret = REFOP_SUCCESS;
	refop_handle_t handle = NULL;

	//dummy data
	char directry[] = "/tmp/refop-test/";
	char file[] = "test.bin";
	char newfile[] = "/tmp/refop-test/test.bin.tmp";
	char latestfile[] = "/tmp/refop-test/test.bin";
	char backupfile[] = "/tmp/refop-test/test.bin.bk1";

	uint8_t *pbuf = NULL;
	uint8_t *prbuf = NULL;
	int64_t sz = 256 * 1024;
	int64_t szr = 0;
	int checker = 0;

	(void)mkdir(directry, 0777);

	pbuf = (uint8_t*)malloc(sz);
	prbuf = (uint8_t*)malloc(sz);

	//short directry string
	ret = refop_create_redundancy_handle(&handle, directry, file);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	memset(pbuf,0xff,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// Operation algorithm
	//     Current                 Next
	//    | latest | backup |     | latest | backup |
	// a1 |    1   |   2    |     |  new   |    1   |
	// a2 |    1   |   x    |     |  new   |    1   |
	// a3 |    x   |   2    |     |  new   |    2   |
	// a4 |    x   |   x    |     |  new   |    x   |

	// a1
	ret = refop_get_redundancy_data(handle, prbuf, sz*2, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xff);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa1,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);


	// a2
	unlink(backupfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz*2, &szr);
	ASSERT_EQ(REFOP_SUCCESS, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa2,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a3
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz*2, &szr);
	ASSERT_EQ(REFOP_RECOVER, ret);
	ASSERT_EQ(sz, szr);
	for(int i=0;i < szr;i++) {
		checker += (prbuf[i] - (uint8_t)0xa1);
	}
	ASSERT_EQ(0, checker);

	memset(pbuf,0xa3,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	// a4
	unlink(backupfile);
	unlink(latestfile);
	ret = refop_get_redundancy_data(handle, prbuf, sz*2, &szr);
	ASSERT_EQ(REFOP_NOENT, ret);

	memset(pbuf,0xa4,sz);
	ret = refop_set_redundancy_data(handle, pbuf, sz);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_remove_redundancy_data(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	ret = refop_release_redundancy_handle(handle);
	ASSERT_EQ(REFOP_SUCCESS, ret);

	free(pbuf);
	free(prbuf);
}
//--------------------------------------------------------------------------------------------------------
