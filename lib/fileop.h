/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	fileop.h
 * @brief	The file operation functions
 */
#ifndef REFOP_FILEOP_H
#define REFOP_FILEOP_H
//-----------------------------------------------------------------------------
#include <librefop.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//-----------------------------------------------------------------------------
struct __attribute__((packed)) s_refop_file_header_v1 {
	uint32_t magic; /*  4 */       /**< Magic code */
	uint32_t version; /*  8 */     /**< Data format version */
	uint32_t version_inv; /* 12 */ /**< Data format version (inversion value) */
	uint16_t crc16; /* 14 */       /**< Data block crc */
	uint16_t crc16_inv; /* 16 */   /**< Data block crc (inversion value) */
	uint64_t size; /* 24 */	       /**< Data block size */
	uint64_t size_inv; /* 32 */    /**< Data block size (inversion value) */
};

#define REFOP_FILE_HEADER_MAGIC ((uint32_t) 0x96962323)
#define REFOP_FILE_HEADER_VERSION_V1 ((uint32_t) 0x00000001)

typedef struct s_refop_file_header_v1 s_refop_file_header;

struct refop_halndle {
	char latestfile[PATH_MAX];  /**< Internal buffer for the latest file name */
	char backupfile1[PATH_MAX]; /**< Internal buffer for the backup file name */
	char newfile[PATH_MAX];	    /**< Internal buffer for the new file name */
	char basedir[PATH_MAX];	    /**< Internal buffer for the file operation base dir */
};

//-----------------------------------------------------------------------------
int refop_new_file_write(refop_handle_t handle, uint8_t *data, int64_t bufsize);
int refop_file_rotation(refop_handle_t handle);
int refop_file_pickup(refop_handle_t handle, uint8_t *data, int64_t bufsize, int64_t *readsize);

//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//-----------------------------------------------------------------------------
#endif //#ifndef REFOP_FILEOP_H
