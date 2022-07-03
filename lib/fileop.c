/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	fileop.c
 * @brief	file operation functions
 */
#include "libredundancyfileop.h"
#include "fileop.h"
#include "file-util.h"
#include "static-configurator.h"
#include "crc16.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


int refop_file_get_with_validation(const char *file, uint8_t *data, int64_t bufsize, int64_t *readsize);
void refop_header_create(s_refop_file_header *head, uint16_t crc16value, uint64_t sizevalue);
int refop_header_validation(const s_refop_file_header *head);
int refop_file_test(const char *filename);

/**
 * Redundancy data write.
 *
 * @param [in]	event	sd event loop handle
 *
 * @return refop_error_t
 * @retval 0 Succeeded.
 * @retval -1 Abnormal fail. Shall not continue.
 * @retval -2 Lager than size limit.
 */
int refop_new_file_write(refop_handle_t handle, uint8_t *data, int64_t bufsize)
{
	struct refop_halndle *hndl = (struct refop_halndle *)handle;
	int ret = -1, fd = -1;
	ssize_t wsize = 0;
	uint8_t *pbuf = NULL, *pdata = NULL;
	uint16_t crc16value = 0;
	int new_state = 0;

	if (bufsize > refop_get_config_data_size_limit() || bufsize <= 0)
		return -2;

	// Fource remove new file - success and noent is ok.
	ret = unlink(hndl->newfile);
	if (ret < 0) {
		if (errno != ENOENT)
			return -1;
	}

	// Create write buffer. To reduce sync write operation
	pbuf = (uint8_t*)malloc(bufsize + sizeof(s_refop_file_header));
	if (pbuf == NULL)
		return -1;

	// Create write data
	pdata = pbuf + sizeof(s_refop_file_header);
	memcpy(pdata, data, bufsize);
	crc16value = crc16(0xffff, pdata, bufsize);

	refop_header_create((s_refop_file_header*)pbuf, crc16value, bufsize);

	fd = open(hndl->newfile, (O_CLOEXEC | O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW), (S_IRUSR | S_IWUSR));
	if (fd < 0) {
		// All open error couldnt recover.
		free(pbuf);
		return -1;
	}

	wsize = safe_write(fd, pbuf, bufsize + sizeof(s_refop_file_header));	
	if (wsize < 0) {
		// All open error couldnt recover.
		(void)close(fd);
		free(pbuf);
		return -1;
	}

	// sync and close
	(void)fsync(fd);
	(void)close(fd);
	free(pbuf);

	return 0;
}


/**
 * Redundancy data write.
 *
 * @param [in]	event	sd event loop handle
 *
 * @return refop_error_t
 * @retval 0 Succeeded.
 * @retval -1 Abnormal fail. Shall not continue.
 */
int refop_file_rotation(refop_handle_t handle)
{
	struct refop_halndle *hndl = (struct refop_halndle *)handle;
	int latest_state = -1, backup_state = -1;
	int fd = -1;

	//Get all file state
	latest_state = refop_file_test(hndl->latestfile);
	backup_state = refop_file_test(hndl->backupfile1);

	if (latest_state == -2 || backup_state == -2)
		return -1;

	// Operation algorithm
	//     Current                 Next
	//    | latest | backup |     | latest | backup |
	// a1 |    1   |   2    |     |  new   |    1   |
	// a2 |    1   |   x    |     |  new   |    1   |
	// a3 |    x   |   2    |     |  new   |    2   |
	// a4 |    x   |   x    |     |  new   |    x   |

	// All error case of file was checked before this point such as stat check and new file create.
	if (latest_state == 0) {
		// a1 or a2
		if (backup_state == 0) {
			//a1
			(void)unlink(hndl->backupfile1);
			(void)rename(hndl->latestfile, hndl->backupfile1);
			(void)rename(hndl->newfile, hndl->latestfile);
		} else {
			//a2
			// nop (void)unlink(hndl->backupfile1);
			(void)rename(hndl->latestfile, hndl->backupfile1);
			(void)rename(hndl->newfile, hndl->latestfile);
		}
	} else {
		//a3 or a4
		if (backup_state == 0) {
			//a3
			// nop (void)unlink(hndl->backupfile1);
			// nop (void)rename(hndl->latestfile, hndl->backupfile1);
			(void)rename(hndl->newfile, hndl->latestfile);
		} else {
			//a4
			// nop (void)unlink(hndl->backupfile1);
			// nop (void)rename(hndl->latestfile, hndl->backupfile1);
			(void)rename(hndl->newfile, hndl->latestfile);
		}
	}

	// directry sync
	fd = open(hndl->basedir, (O_CLOEXEC | O_DIRECTORY | O_NOFOLLOW));
	if (fd >= 0) {
		(void)fsync(fd);
		(void)close(fd);
	}

	return 0;
}

/**
 * Redundancy data write.
 *
 * @param [in]	event	sd event loop handle
 *
 * @return refop_error_t
 * @retval 0 Succeeded.
 * @retval 1 Succeeded with recover.
 * @retval -1 Abnormal fail. Shall not continue.
 * @retval -2 No data.
 * @retval -3 Broaken data.
 */
int refop_file_pickup(refop_handle_t handle, uint8_t *data, int64_t bufsize, int64_t *readsize)
{
	struct refop_halndle *hndl = (struct refop_halndle *)handle;
	int ret1 = -1, ret2 = -1;
	int64_t ressize = 0;


	ret1 = refop_file_get_with_validation(hndl->latestfile, data, bufsize, &ressize);
	if (ret1 == 0) {
		// got valid data
		(*readsize) = ressize;
		return 0;
	} else if (ret1 < -1) {
		// latest file was broaken, file remove
		(void)unlink(hndl->latestfile);
	}

	ret2 = refop_file_get_with_validation(hndl->backupfile1, data, bufsize, &ressize);
	if (ret2 == 0) {
		// got valid data
		(*readsize) = ressize;
		return 1;
	} else if (ret2 < -1) {
		// latest file was broaken, file remove
		(void)unlink(hndl->latestfile);
	}

	if (ret1 == -1 && ret2 == -1)
		return -2;

	return -3;
}

/**
 * Target file status check
 *
 * @param [in]	filename	Target file path
 *
 * @return int
 * @retval 0 Target file is available.
 * @retval -1 No target file.
 * @retval -2 Abnormal fail.
 */
int refop_file_test(const char *filename)
{
	struct stat sb;
	int ret = -1;

	//Check a directry
	ret = stat(filename, &sb);
	if (ret < 0) {
		if (errno == ENOENT)
			return -1;
		else
			return -2;
	}

	return 0;
}

/**
 * Redundancy data write.
 *
 * @param [in]	event	sd event loop handle
 *
 * @return int
 * @retval  0 succeeded.
 * @retval -1 No file entry.
 * @retval -2 Invalid file size.
 * @retval -3 Invalid header.
 * @retval -4 Abnomal request size (smaller than real size)
 * @retval -5 Invalid data.
 * @retval -6 Abnomal file responce.
 */
int refop_file_get_with_validation(const char *file, uint8_t *data, int64_t bufsize, int64_t *readsize)
{
	s_refop_file_header head = {0};
	uint8_t *pbuf = NULL, *pmalloc = NULL;
	uint16_t crc16value = 0;
	ssize_t size = 0;
	int result = -1,ret = -1;
	int fd = -1;

	fd = open(file, (O_CLOEXEC | O_RDONLY | O_NOFOLLOW));
	if (fd < 0) {
		if (errno == ENOENT)
			return -1;
		else 
			return -6;
	}
	
	size = safe_read(fd, &head, sizeof(head));
	if (size != sizeof(head)) {
		ret = -2;
		goto invalid;
	}
	
	result = refop_header_validation(&head);
	if (result != 0) {
		ret = -3;
		goto invalid;
	}

	if (head.size > bufsize) {
		if (head.size <= refop_get_config_data_size_limit()) {
			pmalloc = (uint8_t*)malloc(head.size);
			pbuf = pmalloc;
		} else {
			ret = -4;
			goto invalid;
		}
	} else {
		pbuf = data;
	}

	size = safe_read(fd, pbuf, (size_t)head.size);
	if (size != head.size) {
		ret = -2;
		goto invalid;
	}

	crc16value = crc16(0xffff, pbuf, head.size);
	if (head.crc16 != crc16value) {
		ret = -5;
		goto invalid;
	}

	if (pmalloc != NULL) {
		memcpy(data, pmalloc, bufsize);
		free(pmalloc);
		pmalloc = NULL;
		(*readsize) = bufsize;
	} else
		(*readsize) = head.size;

	(void)close(fd);

	return 0;

invalid:
	free(pmalloc);	//free is NULL safe
	
	if (fd != -1)
		(void)close(fd);

	return ret;
}

/**
 * The refop header create
 *
 * @param [in]	head	The memory of file header.
 * @param [in]	crc16value	The crc value of data block.
 * @param [in]	sizevalue	The size of data block.
 */
void refop_header_create(s_refop_file_header *head, uint16_t crc16value, uint64_t sizevalue)
{
	head->magic = REFOP_FILE_HEADER_MAGIC;

	head->version = REFOP_FILE_HEADER_VERSION_V1;
	head->version_inv = ~head->version;

	head->crc16 = crc16value;
	head->crc16_inv = ~head->crc16;

	head->size = sizevalue;
	head->size_inv = ~head->size;
}

/**
 * The refop header validation
 *
 * @param [in]	head The memory of file header.
 *
 * @return int
 * @retval  0 succeeded.
 * @retval -1 Invalid header.
 */
int refop_header_validation(const s_refop_file_header *head)
{
	int ret = -1;

	//magic check
	if (head->magic != (uint32_t)REFOP_FILE_HEADER_MAGIC)
		goto invalid;

	if (head->version == (uint32_t)(~head->version_inv)) {
		if (head->version != REFOP_FILE_HEADER_VERSION_V1)
			goto invalid;
	} else 
		goto invalid;

	if (head->crc16 != (uint16_t)(~head->crc16_inv))
		goto invalid;

	if (head->size != (uint64_t)(~head->size_inv))
		goto invalid;

	ret = 0;
	
invalid:
	return ret;
}
