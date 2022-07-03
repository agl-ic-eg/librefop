/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	libredundancyfileop.c
 * @brief	The redundancy file operation library
 */
#include "libredundancyfileop.h"
#include "fileop.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

const char c_bk1_suffix[] = ".bk1";
const char c_new_suffix[] = ".tmp";

/**
 * Handle create function of refop.
 *
 * @param [out]	handle	Created refophandle
 * @param [in]	directry	Terget directry
 * @param [in]	filename	Target file name.
 *
 * @return refop_error_t
 * @retval REFOP_SUCCESS This operation was succeeded.
 * @retval REFOP_NOENT The target file/directroy was nothing.
 * @retval REFOP_ARGERROR Argument error.
 * @retval REFOP_SYSERROR Internal operation was failed such as no memory, no disk space and etc.
 */
refop_error_t refop_create_redundancy_handle(refop_handle_t *handle, const char *directry, const char *filename)
{
	struct stat sb;
	struct refop_halndle *hndl = NULL;
	int ret = -1;
	size_t dirlen = 0, filelen = 0;
	refop_error_t refop_error = REFOP_SYSERROR;

	if ((handle == NULL) || (directry == NULL) || (filename == NULL))
		return REFOP_ARGERROR;

	//Check a directry
	ret = stat(directry, &sb);
	if (ret < 0) {
		if ((errno == EACCES) || (errno == ELOOP) || (errno == ENOENT) || (errno == ENOTDIR))
			return REFOP_NOENT;
		else if (errno == ENAMETOOLONG)
			return REFOP_ARGERROR;
		else
			return REFOP_SYSERROR;
	}

	//Handle memory allocate
	hndl = (struct refop_halndle*)malloc(sizeof(struct refop_halndle));
	if (hndl == NULL)
		return REFOP_SYSERROR;
	memset(hndl, 0, sizeof(struct refop_halndle));

	//Create file path
	dirlen = strnlen(directry,PATH_MAX);
	filelen = strnlen(filename,PATH_MAX);
	if ((dirlen + filelen + 10 + 1) > PATH_MAX || (dirlen == 0) || (filelen == 0)) { //file suffix = max 10 byte, / = max 1 byte
		// Path error
		free(hndl);
		return REFOP_ARGERROR;
	}

	// string length was checked, safe.
	(void)strncpy(hndl->latestfile, directry, PATH_MAX);
	if (hndl->latestfile[dirlen-1] != '/')
		(void)strcat(hndl->latestfile, "/");

	(void)strncpy(hndl->basedir, hndl->latestfile, PATH_MAX);

	(void)strcat(hndl->latestfile, filename);

	(void)strncpy(hndl->backupfile1, hndl->latestfile, PATH_MAX);
	(void)strcat(hndl->backupfile1, c_bk1_suffix);

	(void)strncpy(hndl->newfile, hndl->latestfile, PATH_MAX);
	(void)strcat(hndl->newfile, c_new_suffix);

	(*handle) = hndl;

	return REFOP_SUCCESS;
}
/**
 * Handle release function of refop.
 *
 * @param [in]	handle	Refop handle
 *
 * @return refop_error_t
 * @retval REFOP_SUCCESS This operation was succeeded.
 * @retval REFOP_ARGERROR Argument error.
 */
refop_error_t refop_release_redundancy_handle(refop_handle_t handle)
{
	if (handle == NULL)
		return REFOP_ARGERROR;
	
	free(handle);

	return REFOP_SUCCESS;
}

/**
 * The data set function of refop.
 *
 * @param [in]	handle	Refop handle
 * @param [in]	data	Write data for set data.
 * @param [in]	datasize	Write data size (byte).
 *
 * @return refop_error_t
 * @retval REFOP_SUCCESS This operation was succeeded.
 * @retval REFOP_ARGERROR Argument error.
 * @retval REFOP_SYSERROR Internal operation was failed such as no memory, no disk space and etc.
 */
refop_error_t refop_set_redundancy_data(refop_handle_t handle, uint8_t *data, int64_t datasize)
{
	struct refop_halndle *hndl = (struct refop_halndle *)handle;
	int ret = -1;

	if (handle == NULL || data == NULL || datasize < 0)
		return REFOP_ARGERROR;

	ret = refop_new_file_write(handle, data, datasize);
	if (ret < 0) {
		if (ret == -1)
			return REFOP_SYSERROR;
		else
			return REFOP_ARGERROR;
	}

	ret = refop_file_rotation(handle);
	if (ret < 0) {
		(void)unlink(hndl->newfile);
		return REFOP_SYSERROR;
	}

	return REFOP_SUCCESS;
}
/**
 * The data get function of refop.
 *
 * @param [in]	handle	Refop handle
 * @param [in]	data	Read buffer for get data.
 * @param [in]	datasize	Read buffer size (byte).
 * @param [out]	getsize	Readed size (byte).
 *
 * @return refop_error_t
 * @retval REFOP_SUCCESS This operation was succeeded.
 * @retval REFOP_RECOVER This operation was succeeded within recovery.
 * @retval REFOP_NOENT The target file/directroy was nothing.
 * @retval REFOP_BROAKEN This operation was failed. Because all recovery method was failed.
 * @retval REFOP_ARGERROR Argument error.
 * @retval REFOP_SYSERROR Internal operation was failed such as no memory, no disk space and etc.
 */

refop_error_t refop_get_redundancy_data(refop_handle_t handle, uint8_t *data, int64_t datasize, int64_t *getsize)
{
	refop_error_t result = REFOP_SYSERROR;
	int ret = -1;

	if (handle == NULL || data == NULL || datasize < 0 || getsize == NULL)
		return REFOP_ARGERROR;

	ret = refop_file_pickup(handle, data, datasize, getsize);
	if (ret == 0)
		result = REFOP_SUCCESS;
	else if (ret == 1)
		result = REFOP_RECOVER;
	else if (ret == -2)
		result = REFOP_NOENT;
	else if (ret == -3)
		result = REFOP_BROAKEN;
	else
		result = REFOP_SYSERROR;
	
	return result;
}

/**
 * The function of refop all file clean.
 *
 * @param [in]	handle	refop handle
 *
 * @return refop_error_t
 * @retval REFOP_SUCCESS This operation was succeeded.
 * @retval REFOP_ARGERROR Argument error.
 * @retval REFOP_SYSERROR Internal operation was failed such as no memory, no disk space and etc.
 */
refop_error_t refop_remove_redundancy_data(refop_handle_t handle)
{
	struct refop_halndle *hndl = (struct refop_halndle *)handle;
	refop_error_t errorret = REFOP_SUCCESS;
	int ret = -1;

	if (handle == NULL)
		return REFOP_ARGERROR;

	ret = unlink(hndl->newfile);
	if (ret < 0) {
		if (errno != ENOENT)
			errorret = REFOP_SYSERROR;
	}

	ret = unlink(hndl->latestfile);
	if (ret < 0) {
		if (errno != ENOENT)
			errorret = REFOP_SYSERROR;
	}

	ret = unlink(hndl->backupfile1);
	if (ret < 0) {
		if (errno != ENOENT)
			errorret = REFOP_SYSERROR;
	}

	return errorret;
}
