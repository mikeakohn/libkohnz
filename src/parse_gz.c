/**
 *  libkohnz
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2018 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "deflate_codes.h"
#include "gzip.h"

struct _bits
{
  uint32_t holding;
  int length;
};

int read_bits(FILE *in, struct _bits *bits)
{
  int ch = getc(in);

  if (ch == EOF) { return -1; }

  bits->holding <<= 8;
  bits->holding |= deflate_reverse[ch];
  bits->length += 8;

  return 0;
}

#if 0
static uint16_t read_int16(FILE *in)
{
  uint32_t n;

  n = getc(in);
  n |= getc(in) << 8;

  return n;
}
#endif

static uint16_t read_int32(FILE *in)
{
  uint32_t n;

  n = getc(in);
  n |= getc(in) << 8;
  n |= getc(in) << 16;
  n |= getc(in) << 24;

  return n;
}

static int print_string(FILE *in)
{
  int ch;

  while(1)
  {
    ch = getc(in);
    if (ch == 0 || ch == EOF) { break; }
    printf("%c", ch);
  }

  printf("\n");

  return 0;
}

static int print_header(FILE *in, struct _gzip_header *gzip_header)
{
  char flags_text[64];
  char comp_text[64];

  gzip_header->magic[0] = getc(in);
  gzip_header->magic[1] = getc(in);
  gzip_header->compression = getc(in);
  gzip_header->flags = getc(in);
  gzip_header->timestamp = read_int32(in);
  gzip_header->compression_flags = getc(in);
  gzip_header->operating_system = getc(in);

  flags_text[0] = 0;

  if ((gzip_header->flags & 0x01) != 0) { strcat(flags_text, " FTEXT"); }
  if ((gzip_header->flags & 0x02) != 0) { strcat(flags_text, " FHCRC"); }
  if ((gzip_header->flags & 0x04) != 0) { strcat(flags_text, " FEXTRA"); }
  if ((gzip_header->flags & 0x08) != 0) { strcat(flags_text, " FNAME"); }
  if ((gzip_header->flags & 0x10) != 0) { strcat(flags_text, " FCOMMENT"); }

  if ((gzip_header->compression_flags & 0x02) != 0) { strcat(comp_text, " MAX"); }
  if ((gzip_header->compression_flags & 0x04) != 0) { strcat(comp_text, " FAST"); }

  if (gzip_header->magic[0] != 0x1f || gzip_header->magic[1] != 0x8b)
  {
    return -1;
  }

  printf(" == gzip header ==\n");
  printf("            magic: %02x %02x\n",
    gzip_header->magic[0],
    gzip_header->magic[1]);
  printf("      compression: %02x %s\n",
    gzip_header->compression,
    gzip_header->compression == 8 ? "deflate" : "unknown");
  printf("            flags: 0x%02x%s\n", gzip_header->flags, flags_text);
  printf("        timestamp: %d\n", gzip_header->timestamp);
  printf("compression_flags: 0x%02x%s\n", gzip_header->flags, comp_text);
  printf(" operating_system: 0x%02x\n", gzip_header->operating_system);
  printf("\n");

  if ((gzip_header->flags & 0x08) != 0)
  {
    printf("            FNAME: ");
    print_string(in);
  }

  if ((gzip_header->flags & 0x10) != 0)
  {
    printf("         FCOMMENT: ");
    print_string(in);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  FILE *in;
  struct _gzip_header gzip_header;
  uint8_t compression_type = 3;
  struct _bits bits;
  uint32_t code;

  if (argc != 2)
  {
    printf("Usage: %s <file.gz>\n", argv[0]);
    exit(0);
  }

  in = fopen(argv[1], "rb");

  if (in == NULL)
  {
    printf("Could not open file %s\n", argv[1]);
    exit(1);
  }

  do
  {
    if (print_header(in, &gzip_header) != 0)
    {
      printf("Error: Not a gzip file\n");
      exit(0);
    }

    memset(&bits, 0, sizeof(bits));

    read_bits(in, &bits);

    if (bits.holding == EOF)
    {
      printf("Error: @ %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }

    uint8_t bfinal = (bits.holding >> 7) & 1;
    compression_type = (bits.holding >> 5) & 3;

    const char *type = "error";

    switch(compression_type)
    {
      case 0:
        type = "uncompressed";
        break;
      case 2:
        type = "fixed";
        break;
      case 1:
        type = "dynamic";
        break;
    }

    printf("byte=%02x (%02x)\n", bits.holding, deflate_reverse[bits.holding]);
    printf("  final=%d\n", bfinal);
    printf("   type=%s (%d)\n", type, compression_type);

    //bits.holding >>= 3;
    bits.length -= 3;
  }
  while(0);

  if (compression_type == 1)
  {
    while(1)
    {
//printf("holding=%x length=%d\n", bits.holding, bits.length);
      // Check for code of 7 bits
      if (bits.length < 7)
      {
        if (read_bits(in, &bits) == -1) { break; }
      }

      code = bits.holding >> (bits.length - 7);
      code = code & 0x7f;

      if (code >= 0x00 && code <= 0x17)
      {
        bits.length -= 7;

        printf("code=%x (7) data=%d (%02x)\n", code, code + 256, code + 256);

        if (code == 0) { break; }

        continue;
      }

      // Check for code of 8 bits
      if (bits.length < 8)
      {
        if (read_bits(in, &bits) == -1) { break; }
      }

      code = bits.holding >> (bits.length - 8);
      code = code & 0xff;

      if (code >= 0x30 && code <= 0xbf)
      {
        bits.length -= 8;

        printf("code=%x (8) data=%d (%02x)\n", code, code - 0x30, code - 0x30);
        continue;
      }

      // Check for code of 9 bits
      if (bits.length < 9)
      {
        if (read_bits(in, &bits) == -1) { break; }
      }

      code = bits.holding >> (bits.length - 9);
      code = code & 0x1ff;

      if (code >= 0x190 && code <= 0x1ff)
      {
        bits.length -= 9;

        printf("code=%x (9) data=%d (%02x)\n", code, code - 0x190, code - 0x190);
        continue;
      }

      printf("Error: unknown code %s:%d\n", __FILE__, __LINE__);
      break;
    }
  }

  fclose(in);

  return 0;
}

