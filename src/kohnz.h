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

#ifndef _KOHNZ_H
#define _KOHNZ_H

#include <stdio.h>
#include <stdint.h>

#define MODE_UNCOMPRESSED 0
#define MODE_STATIC HUFFMAN 1
#define MODE_DYNAMIC HUFFMAN 2

struct _huffman
{
  uint8_t length;
  uint16_t code;
};

struct _bits
{
  uint32_t holding;
  int length;
};

struct _kohnz
{
  FILE *out;
  struct _bits bits;
  uint64_t file_size;
  uint32_t crc32;
  int mode;
  int len;
  int literals_length;
  int distances_length;
  struct _huffman literals[286];
  struct _huffman distances[32];
  uint8_t data[65536];
};

void kohnz_init();
struct _kohnz *kohnz_open(const char *filename, const char *fname, const char *fcomment);
int kohnz_close(struct _kohnz *kohnz);
int kohnz_start_uncompressed_block(struct _kohnz *kohnz);
int kohnz_start_fixed_block(struct _kohnz *kohnz, int is_final);

int kohnz_start_dynamic_block(
  struct _kohnz *kohnz,
  int is_final,
  uint16_t *literals_sorted,
  uint16_t *distances_sorted,
  int literals_count,
  int distances_count);

int kohnz_end_fixed_block(struct _kohnz *kohnz);
int kohnz_end_dynamic_block(struct _kohnz *kohnz);
int kohnz_write_uncompressed(struct _kohnz *kohnz, const uint8_t *data, int length);
int kohnz_write_fixed(struct _kohnz *kohnz, const uint8_t *data, int length);
int kohnz_write_dynamic(struct _kohnz *kohnz, const uint8_t *data, int length);
int kohnz_write_fixed_lz77(struct _kohnz *kohnz, int distance, int length);
int kohnz_write_dynamic_lz77(struct _kohnz *kohnz, int distance, int length);
int kohnz_build_crc32(struct _kohnz *kohnz, const uint8_t *data, int length);
uint64_t kohnz_get_offset(struct _kohnz *kohnz);

#endif

