/**
 * SPDX-License-Identifier: Apache-2.0
 *
 * @file	static-configurator.c
 * @brief	static configurator
 */

#include "static-configurator.h"


/** refop static configurator.*/

/**
 * Getter for the data size limit.
 *
 * @return uint64_t	 Maximum data size.
 */
uint64_t refop_get_config_data_size_limit(void)
{
	return (1 * 1024 * 1024); // 1 MByte;
}
