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

#include "gzip.h"

#if 0
static uint16_t read_int16(FILE *in)
{
  uint32_t n;

  n = getc(in);
  n = getc(in) << 8;

  return n;
}
#endif

static uint16_t read_int32(FILE *in)
{
  uint32_t n;

  n = getc(in);
  n = getc(in) << 8;
  n = getc(in) << 16;
  n = getc(in) << 24;

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
    }
  }
  while(0);

  fclose(in);

  return 0;
}

