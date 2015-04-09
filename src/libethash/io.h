/*
  This file is part of ethash.

  ethash is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ethash is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ethash.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file io.h
 * @author Lefteris Karapetsas <lefteris@ethdev.com>
 * @date 2015
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ethash.h"
#include "endian.h"

#ifdef __cplusplus
extern "C" {
#endif

// Maximum size for mutable part of DAG file name
// 10 is for maximum number of digits of a uint32_t (for REVISION)
// 1 is for _ and 16 is for the first 16 hex digits for first 8 bytes of
// the seedhash and last 1 is for the null terminating character
// Reference: https://github.com/ethereum/wiki/wiki/Ethash-DAG
#define DAG_MUTABLE_NAME_MAX_SIZE (10 + 1 + 16 + 1)

/// Possible return values of @see ethash_io_prepare
enum ethash_io_rc {
    ETHASH_IO_FAIL = 0,      ///< There has been an IO failure
    ETHASH_IO_MEMO_MISMATCH, ///< Memo file either did not exist or there was content mismatch
    ETHASH_IO_MEMO_MATCH,    ///< Memo file existed and contents matched. No need to do anything
};

/**
 * Prepares io for ethash
 *
 * Create the DAG directory and the DAG file if they don't exists.
 *
 * @param dirname        A null terminated c-string of the path of the ethash
 *                       data directory. If it does not exist it's created.
 * @param seedhash       The seedhash of the current block number, used in the
 *                       naming of the file as can be seen from the spec at:
 *                       https://github.com/ethereum/wiki/wiki/Ethash-DAG
 * @return               For possible return values @see enum ethash_io_rc
 */
enum ethash_io_rc ethash_io_prepare(char const *dirname, ethash_h256_t seedhash, FILE **f);

static inline bool ethash_io_mutable_name(uint32_t revision,
                                          ethash_h256_t *seed_hash,
                                          char *output)
{
    uint64_t hash = *((uint64_t*)seed_hash);
#if LITTLE_ENDIAN == BYTE_ORDER
    hash = bitfn_swap64(hash);
#endif
    return snprintf(output, DAG_MUTABLE_NAME_MAX_SIZE, "%u_%lx", revision, hash) >= 0;
/* #if BYTE_ORDER == LITTLE_ENDIAN */
/*     return snprintf(output, DAG_MUTABLE_NAME_MAX_SIZE, "%u_%lx", */
/*                     revision, *((uint64_t*)((char*)seed_hash + 24))) >= 0; */
/* #else */
/*     return snprintf(output, DAG_MUTABLE_NAME_MAX_SIZE, "%u_%lx", */
/*                     revision, *((uint64_t*)((char*)seed_hash))) >= 0; */
/* #endif */
}

static inline char *ethash_io_create_filename(char const *dirname,
                                              char const* filename,
                                              size_t filename_length)
{
    // in C the cast is not needed, but a C++ compiler will complain for invalid conversion
    char *name = (char*)malloc(strlen(dirname) + filename_length);
    if (!name) {
        return NULL;
    }

    name[0] = '\0';
    strcat(name, dirname);
    strcat(name, filename);
    return name;
}


#ifdef __cplusplus
}
#endif
