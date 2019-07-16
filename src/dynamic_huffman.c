/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "deflate_codes.h"
#include "dynamic_huffman.h"
#include "fileio.h"

#if 0
static uint8_t coding_map[] =
{
  16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};
#endif

static int build_huffman_values(
  struct _huffman *table,
  int table_length)
{
  uint8_t bl_count[16];
  uint16_t next_code[16];
  int code, n;

  memset(bl_count, 0, sizeof(bl_count));

  // Create a table that counts how many of each length there is.
  for (n = 0; n < table_length; n++)
  {
    if (table[n].length != 0)
    {
      bl_count[table[n].length]++;
    }
  }

  // For each code length the code (starts out as 0 for the smallest
  // code length, which should be 0.  The next code is the last code
  // plus the count of the last length.
  code = 0;

  memset(next_code, 0, sizeof(next_code));

  for (n = 1; n <= 15; n++)
  {
    code = (code + bl_count[n - 1]) << 1;
    next_code[n] = code;
  }

  // Build the huffman table from the codes.
  for (n = 0; n < table_length; n++)
  {
    if (table[n].length != 0)
    {
      const int length = table[n].length;

      table[n].code = next_code[length]++;
    }
  }

  return 0;
}

int dynamic_huffman_build(
  struct _kohnz *kohnz,
  uint16_t *literals_sorted,
  uint16_t *distances_sorted,
  int literals_count,
  int distances_count)
{
  struct _huffman coding[19];
  uint16_t next_code[16];
  int n, length, count, highest_length = 0;
  int used_lengths[16];

  memset(coding, 0, sizeof(coding));
  memset(kohnz->literals, 0, sizeof(kohnz->literals));
  memset(kohnz->distances, 0, sizeof(kohnz->distances));
  memset(next_code, 0, sizeof(next_code));
  memset(used_lengths, 0, sizeof(used_lengths));

  kohnz->literals_length = 0;
  kohnz->distances_length = 0;

  // Compute lengths for the literal table
  length = 2;
  count = 0;

  for (n = 0; n < literals_count; n++)
  {
    const int index = literals_sorted[n];

    if (index + 1 > kohnz->literals_length)
    {
      kohnz->literals_length = index + 1;
    }

    kohnz->literals[n].length = length;
    used_lengths[length]++;

    count++;

    if (count == (1 << (length - 1)) + 1)
    {
      length++;
      count = 0;
    }
  }

  highest_length = length;

  if (kohnz->literals_length < 257) { kohnz->literals_length = 257; }

  // Compute lengths for the distance table
  length = 2;
  count = 0;

  for (n = 0; n < distances_count; n++)
  {
    const int index = distances_sorted[n];

    if (index + 1 > kohnz->distances_length)
    {
      kohnz->distances_length = index + 1;
    }

    kohnz->distances[n].length = length;
    used_lengths[length]++;

    count++;

    if (count == (1 << (length - 1)) + 1)
    {
      length++;
      count = 0;
    }
  }

  if (kohnz->distances_length < 257) { kohnz->distances_length = 1; }

  if (highest_length < length) { highest_length = length; }
  if (used_lengths[highest_length] == 0) { highest_length--; }

  build_huffman_values(kohnz->literals, kohnz->literals_length);
  build_huffman_values(kohnz->distances, kohnz->distances_length);

  int coding_count = 0;

  if (used_lengths[highest_length - 1] > used_lengths[highest_length])
  {
    coding[coding_count++].length = highest_length - 1;
    coding[coding_count++].length = highest_length;
    highest_length -= 2;
  }

  // Find the best sizes for huffman tree that compresses the
  // literal and distance trees.
  for (n = highest_length; n >= 1; n--)
  {
    if (used_lengths[n] == 0) { continue; }
    coding[coding_count++].length = highest_length;
  }

  coding[coding_count++].length = 16;
  coding[coding_count++].length = 17;
  coding[coding_count++].length = 18;

  build_huffman_values(coding, coding_count);

  // Going to waste some bits for now.  Can optimize later, but it seems
  // like a waste of CPU for a few extra bits.
  const int hclen = 19 - 4;
  const int hdist = kohnz->distances_length - 1;
  const int hlit = kohnz->literals_length - 257;

//#if 0
printf("hclen: %d\n", hclen + 4);
printf("hdist: %d\n", hdist + 1);
printf("hlit: %d\n", hlit + 257);
//#endif

  write_bits(kohnz, deflate_reverse[hlit] >> 3, 5);
  write_bits(kohnz, deflate_reverse[hdist] >> 3, 5);
  write_bits(kohnz, deflate_reverse[hclen] >> 4, 4);

  return 0;
}

