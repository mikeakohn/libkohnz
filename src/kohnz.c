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

#include "crc32.h"
#include "deflate_codes.h"
#include "dynamic_huffman.h"
#include "kohnz.h"

static int write16(FILE *out, uint32_t num)
{
  uint8_t data[2];

  data[0] = num;
  data[1] = num >> 8;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

static int write32(FILE *out, uint32_t num)
{
  uint8_t data[4];

  data[0] = num;
  data[1] = num >> 8;
  data[2] = num >> 16;
  data[3] = num >> 24;

  fwrite(data, sizeof(data), 1, out);

  return 0;
}

static void write_bits(struct _kohnz *kohnz, uint32_t data, int length)
{
  struct _bits *bits = &kohnz->bits;

  bits->holding |= data << bits->length;
  bits->length += length;

  while(bits->length >= 8)
  {
    const int byte = bits->holding & 0xff;

    //putc(deflate_reverse[byte], kohnz->out);
    putc(byte, kohnz->out);

    bits->holding >>= 8;
    bits->length -= 8;
  }

#if 0
  bits->holding |= data << bits->length;
  bits->length += length;

  while(bits->length > 8)
  {
    putc(bits->holding, kohnz->out);
    bits->holding >>= 8;
    bits->length -= 8;
  }
#endif
}

static void write_bits_end_block(struct _kohnz *kohnz)
{
  struct _bits *bits = &kohnz->bits;

  if (bits->length == 0) { return; }

  uint8_t data = bits->holding & ((1 << bits->length) -1);
  //putc(deflate_reverse[data], kohnz->out);
  putc(data, kohnz->out);
}

void kohnz_init()
{
  kohnz_crc32_init();
  deflate_length_table_init();
  deflate_distance_table_init();
}

struct _kohnz *kohnz_open(const char *filename, const char *fname, const char *fcomment)
{
  struct _kohnz *kohnz;

  kohnz = (struct _kohnz *)malloc(sizeof(struct _kohnz));

  if (kohnz == NULL) { return NULL; }

  memset(kohnz, 0, sizeof(struct _kohnz));

  kohnz->crc32 = 0xffffffff;

  kohnz->out = fopen(filename, "wb");

  if (kohnz == NULL)
  {
    free(kohnz);
    return NULL;
  }

  uint8_t flags = 0;

  if (fname != NULL && fname[0] != 0) { flags |= 0x08; }
  if (fcomment != NULL && fcomment[0] != 0) { flags |= 0x10; }

  // Magic number
  putc(0x1f, kohnz->out);
  putc(0x8b, kohnz->out);
  // Compression method 8 (DEFLATE)
  putc(0x08, kohnz->out);
  putc(flags, kohnz->out);
  // Timestamp
  write32(kohnz->out, 0);
  // Compression flags
  putc(0x02, kohnz->out);
  // Operating system (3 is Unix)
  putc(0x03, kohnz->out);

  if (fname != NULL && fname[0] != 0)
  {
    fwrite(fname, 1, strlen(fname) + 1, kohnz->out);
  }

  if (fcomment != NULL && fcomment[0] != 0)
  {
    fwrite(fcomment, 1, strlen(fcomment) + 1, kohnz->out);
  }

  return kohnz;
}

int kohnz_close(struct _kohnz *kohnz)
{
  write32(kohnz->out, kohnz->crc32 ^ 0xffffffff);
  write32(kohnz->out, kohnz->file_size);

  fclose(kohnz->out);

  free(kohnz);

  return 0;
}

int kohnz_start_uncompressed_block(struct _kohnz *kohnz)
{
  putc(1, kohnz->out);

  return 0;
}

int kohnz_start_fixed_block(struct _kohnz *kohnz, int is_final)
{
  kohnz->bits.holding = 0;
  kohnz->bits.length = 0;

  // final=1 if this is the last block.
  // type=1, fixed 
  write_bits(kohnz, is_final == 0 ? 0 : 1, 1);
  write_bits(kohnz, 1, 2);

  return 0;
}

int kohnz_start_dynamic_block(
  struct _kohnz *kohnz,
  int is_final,
  uint16_t *literals_sorted,
  uint16_t *distances_sorted,
  int literals_count,
  int distances_count)
{
  kohnz->bits.holding = 0;
  kohnz->bits.length = 0;

  // final=1 if this is the last block.
  // type=2, dynamic
  write_bits(kohnz, is_final == 0 ? 0 : 1, 1);
  write_bits(kohnz, 2, 2);

  return dynamic_huffman_build(kohnz, literals_sorted, distances_sorted, literals_count, distances_count);
}

int kohnz_end_fixed_block(struct _kohnz *kohnz)
{
  // Write literal 256 and close block.
  write_bits(kohnz, 0x00, 7);
  write_bits_end_block(kohnz);

  return 0;
}

int kohnz_end_dynamic_block(struct _kohnz *kohnz)
{
  return -1;
}

int kohnz_write_uncompressed(struct _kohnz *kohnz, const uint8_t *data, int length)
{
  write16(kohnz->out, length);
  write16(kohnz->out, length ^ 0xffff);
  fwrite(data, 1, length, kohnz->out);

  kohnz->crc32 = kohnz_crc32(data, length, kohnz->crc32);
  kohnz->file_size += length;

  return 0;
}

int kohnz_write_fixed(struct _kohnz *kohnz, const uint8_t *data, int length)
{
  int code;
  int n;

  for (n = 0; n < length; n++)
  {
    if (*data <= 143)
    {
      code = deflate_reverse[*data + 0x30];
      write_bits(kohnz, code, 8);
    }
    else if (*data <= 255)
    {
      code = (*data - 144) + 0x190;
      code = (deflate_reverse[code & 0xff] << 1) | ((code >> 8) & 1);

      write_bits(kohnz, code, 9);
    }

    data++;
  }

  kohnz->file_size += length;

  return 0;
}

int kohnz_write_dynamic(struct _kohnz *kohnz, const uint8_t *data, int length)
{
  return -1;
}

int kohnz_write_fixed_lz77(struct _kohnz *kohnz, int distance, int length)
{
  int code;
  int extra_bits;

  code = deflate_length_table[length].code;
  extra_bits = deflate_length_table[length].extra_bits;

#if 0
printf("length=%d code=%d extra_bits=%d  %x\n",
  length,
  code,
  extra_bits,
  length - deflate_length_codes[code - 257]);
#endif

  if (code < 256)
  {
    return -3;
  }
  else if (code <= 279)
  {
    write_bits(kohnz, deflate_reverse[(code - 256) + 0x00] >> 1, 7);
  }
  else if (code <= 287)
  {
    write_bits(kohnz, deflate_reverse[(code - 280) + 0xc0], 8);
  }

  if (extra_bits != 0)
  {
    write_bits(kohnz, length - deflate_length_codes[code - 257], extra_bits);
  }

  code = deflate_distance_table[distance - 1].code;
  extra_bits = deflate_distance_table[distance - 1].extra_bits;

#if 0
printf("distance=%d code=%d extra_bits=%d  %x\n",
  distance,
  code,
  extra_bits,
  distance - deflate_distance_codes[code]);
#endif

  write_bits(kohnz, deflate_reverse[code] >> 3, 5);

  if (extra_bits != 0)
  {
    //if (extra_bits <= 8)
    {
      write_bits(kohnz, distance - deflate_distance_codes[code], extra_bits);
    }
#if 0
    else
    {
      int diff = distance - deflate_distance_codes[code];

printf("code=%d distance=%d distance_code=%d diff=%d\n", code, distance, deflate_distance_codes[code], diff);
      write_bits(kohnz, diff & 0xff, 8);
      write_bits(kohnz, diff >> 8, extra_bits - 8);
    }
#endif
  }

  kohnz->file_size += length;

  return 0;
}

int kohnz_write_dynamic_lz77(struct _kohnz *kohnz, int distance, int length)
{
  return -1;
}

int kohnz_build_crc32(struct _kohnz *kohnz, const uint8_t *data, int length)
{
  kohnz->crc32 = kohnz_crc32(data, length, kohnz->crc32);

  return 0;
}

uint64_t kohnz_get_offset(struct _kohnz *kohnz)
{
  return kohnz->file_size;
}

