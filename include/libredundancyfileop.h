/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	libredundancyfileop.h
 * @brief	Interface header for the redundancy file operation library
 */
#ifndef LIBREDUNDANCY_FILEOP_H
#define LIBREDUNDANCY_FILEOP_H
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
/**
 * Error code and type definition
 * @enum refop_error_t
 */
typedef enum refop_error {
	//! This operation was succeeded.
	REFOP_SUCCESS = 0,

	//! This operation was succeeded within recovery.
	REFOP_RECOVER = 1,

	//! The target file/directroy was nothing.
	REFOP_NOENT = -1,

	//! This operation was failed. Because all recovery method was failed.
	REFOP_BROAKEN = -2,

	//! Argument error.
	REFOP_ARGERROR = -3,

	//! Internal operation was failed such as no memory, no disk space and etc.
	REFOP_SYSERROR = -100,

} refop_error_t;
//-----------------------------------------------------------------------------
typedef struct refop_halndle *refop_handle_t;

//-----------------------------------------------------------------------------
refop_error_t refop_create_redundancy_handle(refop_handle_t *handle, const char *directry, const char *filename);
refop_error_t refop_release_redundancy_handle(refop_handle_t handle);
refop_error_t refop_set_redundancy_data(refop_handle_t handle, uint8_t *data, int64_t datasize);
refop_error_t refop_get_redundancy_data(refop_handle_t handle, uint8_t *data, int64_t datasize, int64_t *getsize);
refop_error_t refop_remove_redundancy_data(refop_handle_t handle);

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif //#ifndef LIBREDUNDANCY_FILEOP_H
