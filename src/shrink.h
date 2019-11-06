/*
 * shrink.h - compressor definitions
 *
 * Copyright (C) 2019 Emmanuel Marty
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/*
 * Uses the libdivsufsort library Copyright (c) 2003-2008 Yuta Mori
 *
 * Inspired by cap by Sven-�ke Dahl. https://github.com/svendahl/cap
 * Also inspired by Charles Bloom's compression blog. http://cbloomrants.blogspot.com/
 * With ideas from LZ4 by Yann Collet. https://github.com/lz4/lz4
 * With help and support from spke <zxintrospec@gmail.com>
 *
 */

#ifndef _SHRINK_H
#define _SHRINK_H

#include "divsufsort.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCP_BITS 10
#define LCP_MAX (1U<<(LCP_BITS - 1))
#define LCP_SHIFT (31-LCP_BITS)
#define LCP_MASK (((1U<<LCP_BITS) - 1) << LCP_SHIFT)
#define POS_MASK ((1U<<LCP_SHIFT) - 1)
#define VISITED_FLAG 0x80000000
#define EXCL_VISITED_MASK  0x7fffffff

#define NMATCHES_PER_ARRIVAL 8
#define MATCHES_PER_ARRIVAL_SHIFT 3

#define NMATCHES_PER_INDEX 64
#define MATCHES_PER_INDEX_SHIFT 6

#define LEAVE_ALONE_MATCH_SIZE 120

/** One match option */
typedef struct _apultra_match {
   unsigned int length:10;
   unsigned int offset:21;
} apultra_match;

/** One finalized match */
typedef struct _apultra_final_match {
   int length;
   int offset;
} apultra_final_match;

/** Forward arrival slot */
typedef struct {
   int cost;
   int from_pos;
   char from_slot;
   char follows_literal;

   unsigned short rep_len;
   int rep_offset;
   int rep_pos;
   int score;

   int match_offset;
   int match_len;
} apultra_arrival;

/** Compression statistics */
typedef struct _apultra_stats {
   int num_literals;
   int num_4bit_matches;
   int num_7bit_matches;
   int num_variable_matches;
   int num_rep_matches;

   int min_offset;
   int max_offset;
   long long total_offsets;

   int min_match_len;
   int max_match_len;
   int total_match_lens;

   int min_rle1_len;
   int max_rle1_len;
   int total_rle1_lens;

   int min_rle2_len;
   int max_rle2_len;
   int total_rle2_lens;

   int commands_divisor;
   int match_divisor;
   int rle1_divisor;
   int rle2_divisor;
} apultra_stats;

/** Compression context */
typedef struct _apultra_compressor {
   divsufsort_ctx_t divsufsort_context;
   unsigned int *intervals;
   unsigned int *pos_data;
   unsigned int *open_intervals;
   apultra_match *match;
   apultra_final_match *best_match;
   apultra_arrival *arrival;
   int flags;
   apultra_stats stats;
} apultra_compressor;

/**
 * Get maximum compressed size of input(source) data
 *
 * @param nInputSize input(source) size in bytes
 *
 * @return maximum compressed size
 */
size_t apultra_get_max_compressed_size(size_t nInputSize);

/**
 * Compress memory
 *
 * @param pInputData pointer to input(source) data to compress
 * @param pOutBuffer buffer for compressed data
 * @param nInputSize input(source) size in bytes
 * @param nMaxOutBufferSize maximum capacity of compression buffer
 * @param nFlags compression flags (set to 0)
 * @param progress progress function, called after compressing each block, or NULL for none
 * @param pStats pointer to compression stats that are filled if this function is successful, or NULL
 *
 * @return actual compressed size, or -1 for error
 */
size_t apultra_compress(const unsigned char *pInputData, unsigned char *pOutBuffer, size_t nInputSize, size_t nMaxOutBufferSize,
   const unsigned int nFlags, void(*progress)(long long nOriginalSize, long long nCompressedSize), apultra_stats *pStats);

#ifdef __cplusplus
}
#endif

#endif /* _SHRINK_H */