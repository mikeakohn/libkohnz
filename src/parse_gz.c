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
#include <stdint.h>
#include <string.h>

#include "deflate_codes.h"
#include "gzip.h"
#include "kohnz.h"

static int read_bits(FILE *in, struct _bits *bits)
{
  int ch = getc(in);

  if (ch == EOF) { return -1; }

  bits->holding <<= 8;
  bits->holding |= deflate_reverse[ch];
  bits->length += 8;

  return 0;
}

static int get_bits(FILE *in, struct _bits *bits, int length)
{
  int data;

  if (bits->length < length)
  {
    // Not good.
    if (read_bits(in, bits) == -1) { return 0; }
  }

  data = bits->holding >> (bits->length - length);
  data = data & ((1 << length) - 1);

  bits->length -= length;

  return data;
}

static void convert_binary(char *s, int num, int length)
{
  int bit = 1 << (length - 1);
  int n;

  for (n = 0; n < length; n++)
  {
    *s = (num & bit) == 0 ? '0' : '1';
    bit = bit >> 1;
    s++;
  }

  *s = 0;
}

static uint16_t read_int16(FILE *in)
{
  uint32_t n;

  n = getc(in);
  n |= getc(in) << 8;

  return n;
}

static uint32_t read_int32(FILE *in)
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
  comp_text[0] = 0;

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

int inflate_uncompressed(FILE *in)
{
  int len = read_int16(in);
  int nlen = read_int16(in);

  printf("        len=%d nlen=%d (%d)\n", len, nlen, nlen ^ 0xffff);

  fseek(in, len, SEEK_CUR);

  printf("      crc32=%x\n", read_int32(in));
  printf("file_length=%d\n", read_int32(in));

  return 0;
}

int inflate_fixed_huffman(FILE *in, struct _bits *bits)
{
  uint32_t code, literal;
  int length;

  while(1)
  {
    code = 0;
    literal = 0;
    length = 0;

    do
    {
      // Check for code of 7 bits
      if (bits->length < 7)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      code = bits->holding >> (bits->length - 7);
      code = code & 0x7f;

      if (code >= 0x00 && code <= 0x17)
      {
        literal = code + 256;
        bits->length -= 7;
        length = 7;
        break;
      }

      // Check for code of 8 bits
      if (bits->length < 8)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      code = bits->holding >> (bits->length - 8);
      code = code & 0xff;

      if (code >= 0x30 && code <= 0xbf)
      {
        literal = code - 0x30;
        bits->length -= 8;
        length = 8;
        continue;
      }

      if (code >= 0xc0 && code <= 0xc7)
      {
        literal = (code - 0xc7) + 280;
        bits->length -= 8;
        length = 8;
        continue;
      }

      // Check for code of 9 bits
      if (bits->length < 9)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      code = bits->holding >> (bits->length - 9);
      code = code & 0x1ff;

      if (code >= 0x190 && code <= 0x1ff)
      {
        literal = code - 0x190;
        bits->length -= 9;
        length = 9;
        continue;
      }

      printf("Error: unknown code %s:%d\n", __FILE__, __LINE__);
      return -1;
    } while(0);

    char c = ' ';

    if (literal > ' ' && literal < 'z') { c = literal; }

    printf("code=%x (%d) literal=%d (%02x %c)\n", code, length, literal, literal, c);

    if (literal == 256) { break; }

    if (literal > 256)
    {
      int length_code = literal - 257;
      int extra_bits = deflate_length_extra_bits[length_code];
      int length = deflate_length_codes[length_code];
      int data;

      data = 0;

      if (extra_bits != 0)
      {
        if (bits->length < extra_bits)
        {
          if (read_bits(in, bits) == -1) { break; }
        }

        data = bits->holding >> (bits->length - extra_bits);
        data = data & ((1 << extra_bits) - 1);
        bits->length -= extra_bits;

        data = deflate_reverse[data] >> (8 - extra_bits);

        length += data;
      }

      // printf("   -- literal=%d length_code=%d length=%d extra_bits=%d extra_data=%d\n", literal, length_code, length - data, extra_bits, data);

      data = 0;

      if (bits->length < 5)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      int distance_code = bits->holding >> (bits->length - 5);
      distance_code = distance_code & 0x1f;

      bits->length -= 5;

      int distance = deflate_distance_codes[distance_code];
      extra_bits = deflate_distance_extra_bits[distance_code];

      if (extra_bits != 0)
      {
        if (bits->length < extra_bits)
        {
          if (read_bits(in, bits) == -1) { break; }
        }

        data = bits->holding >> (bits->length - extra_bits);
        data = data & ((1 << extra_bits) - 1);
        bits->length -= extra_bits;

        if (extra_bits <= 8)
        {
          data = deflate_reverse[data] >> (8 - extra_bits);
        }
        else
        {
          data = deflate_reverse[data & 0xff] << 8 | deflate_reverse[data >> 8];
          data = data >> (16 - extra_bits);
        }

        distance += data;
      }

      // printf("   -- distance_code=%d distance=%d extra_bits=%d extra_data=%d\n", distance_code, distance - data, extra_bits, data);

      printf("  [ length=%d distance=%d ]\n", length, distance);
    }
  }

  printf("      crc32=%x\n", read_int32(in));
  printf("file_length=%d\n", read_int32(in));

  return 0;
}

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

static int build_huffman_table(
  FILE *in,
  struct _bits *bits,
  struct _huffman *coding,
  struct _huffman *literals,
  struct _huffman *distances,
  int hlit_count,
  int hdist_count)
{
  int n, r;
  int previous = 0;
  const int table_length = hlit_count + hdist_count;

  bits->holding &= (1 << bits->length) - 1;

  // Get the lengths of all the codes in the literal table.
  r = 0;

  while (r < table_length)
  {
    for (n = 0; n < 19; n++)
    {
      if (coding[n].length == 0) { continue; }

      // Find value of next code in stream.
      if (bits->length < coding[n].length)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      const int code = bits->holding >> (bits->length - coding[n].length);

      if (code == coding[n].code) { break; }
    }

    if (n == 19)
    {
      printf("Unknown code for literal %d\n", r);
      return -1;
    }

    bits->length -= coding[n].length;
    bits->holding &= (1 << bits->length) - 1;

    if (n <= 15)
    {
      printf("%d literal=%d\n", r, n);

      previous = n;

      // For code 0 to 15 the length is simply the code. 
      if (r < hlit_count)
      {
        literals[r++].length = n;
      }
        else
      {
        distances[r - hlit_count].length = n;
        r++;
      }
    }
      else
    if (n == 16)
    {
      // For code 16 copy the previous value 3 to 6 times (need 2 more bits).

      if (bits->length < 2)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      int count = bits->holding >> (bits->length - 2);
      bits->length -= 2;
      bits->holding &= (1 << bits->length) - 1;

      count = deflate_reverse[count] >> 6;
      count += 3;

      printf("%d repeat=%d for %d times\n", r, previous, count);

      for (n = 0; n < count; n++)
      {
        if (r < hlit_count)
        {
          literals[r++].length = previous;
        }
          else
        {
          distances[r - hlit_count].length = previous;
          r++;
        }
      }
    }
      else
    if (n == 17)
    {
      // For code 17 repeat a code length of 0 for 3 to 10 times
      // (need 3 more bits).

      if (bits->length < 3)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      int count = bits->holding >> (bits->length - 3);
      bits->length -= 3;
      bits->holding &= (1 << bits->length) - 1;

      count = deflate_reverse[count] >> 5;
      count += 3;

      printf("%d clear=%d\n", r, count);

      for (n = 0; n < count; n++)
      {
        if (r < hlit_count)
        {
          literals[r++].length = 0;
        }
          else
        {
          distances[r - hlit_count].length = 0;
          r++;
        }
      }
    }
      else
    if (n == 18)
    {
      // For code 18 repeat a code length of 0 for 11 to 138 times
      // (need 7 more bits).

      if (bits->length < 7)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      int count = bits->holding >> (bits->length - 7);
      bits->length -= 7;
      bits->holding &= (1 << bits->length) - 1;

      count = deflate_reverse[count] >> 1;
      count += 11;

      printf("%d clear=%d\n", r, count);

      for (n = 0; n < count; n++)
      {
        if (r < hlit_count)
        {
          literals[r++].length = 0;
        }
          else
        {
          distances[r - hlit_count].length = 0;
          r++;
        }
      }
    }
  }

  printf("r=%d/%d\n", r, hlit_count + hdist_count);

  build_huffman_values(literals, hlit_count);
  build_huffman_values(distances, hdist_count);

  return 0;
}

int inflate_dynamic_huffman(FILE *in, struct _bits *bits)
{
  struct _huffman coding[19];
  struct _huffman literals[286];
  struct _huffman distances[32];
  uint8_t bl_count[16];
  uint8_t next_code[16];
  int code, n;

  memset(coding, 0, sizeof(coding));
  memset(literals, 0, sizeof(literals));
  memset(distances, 0, sizeof(distances));

  // 5 Bits: HLIT, # of Literal/Length codes - 257 (257 - 286)
  // 5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
  // 4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
  int hlit_count = (deflate_reverse[get_bits(in, bits, 5)] >> 3) + 257;
  int hdist_count = (deflate_reverse[get_bits(in, bits, 5)] >> 3) + 1;
  int hclen_count = (deflate_reverse[get_bits(in, bits, 4)] >> 4) + 4;

  printf("  HLIT: %d\n", hlit_count);
  printf(" HDIST: %d\n", hdist_count);
  printf(" HCLEN: %d\n", hclen_count);

  // Build the huffman code table for decoding the HLIT / HDIST
  // table.

  // First, for every code (0 to 19) read from the file the
  // bit lengths of the codes.
  for (n = 0; n < hclen_count; n++)
  {
    int length = (deflate_reverse[get_bits(in, bits, 3)] >> 5);

    coding[deflate_hclen_map[n]].length = length;
  }

  memset(bl_count, 0, sizeof(bl_count));

  // Create a table that counts how many of each length there is.
  for (n = 0; n < 19; n++)
  {
    if (coding[n].length != 0)
    {
      //printf("  %d: len=%d\n", n, coding[n].length);
      bl_count[coding[n].length]++;
    }
  }

  // For each code length the code (starts out as 0 for the smallest
  // code length, which should be 0.  The next code is the last code
  // plus the count of the last length.
  code = 0;

  memset(next_code, 0, sizeof(next_code));

  for (n = 1; n <= 7; n++)
  {
    code = (code + bl_count[n - 1]) << 1;
    next_code[n] = code;
  }

  printf(" -- Huffman code coding table --\n");

  // Build the table from the codes.
  for (n = 0; n < 19; n++)
  {
    char temp[16];

    if (coding[n].length != 0)
    {
      const int length = coding[n].length;

      coding[n].code = next_code[length]++;

      // Debugging: Print value to screen in binary format.
      convert_binary(temp, coding[n].code, length);
      printf("  %d: len=%d code=%s\n", n, length, temp);
    }
  }

  build_huffman_table(in, bits, coding, literals, distances, hlit_count, hdist_count);

  printf(" -- Huffman code literal table --\n");

  for (n = 0; n < 286; n++)
  {
    char temp[16];

    if (literals[n].length != 0)
    {
      const int length = literals[n].length;

      // Debugging: Print value to screen in binary format.
      convert_binary(temp, literals[n].code, length);
      printf("  %d: len=%d code=%s\n", n, length, temp);
    }
  }

  printf(" -- Huffman code distance table --\n");

  for (n = 0; n < 32; n++)
  {
    char temp[16];

    if (distances[n].length != 0)
    {
      const int length = distances[n].length;

      convert_binary(temp, distances[n].code, length);
      printf("  %d: len=%d code=%s\n", n, length, temp);
    }
  }

#if 0
printf(">> holding=%04x length=%d\n",
  bits->holding,
  bits->length);
#endif

  while(1)
  {
    for (n = 0; n < hlit_count; n++)
    {
      if (literals[n].length == 0) { continue; }

      // Find value of next code in stream.
      while (bits->length < literals[n].length)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

      if (bits->length < literals[n].length) { break; }

      const int code = bits->holding >> (bits->length - literals[n].length);

      if (code == literals[n].code) { break; }
    }

#if 0
printf("holding=%04x length=%d literal=%d\n",
  bits->holding,
  bits->length,
  n);
#endif

    if (n == hlit_count)
    {
      printf("Unknown code for literal %d\n", n);
      return -1;
    }

    bits->length -= literals[n].length;
    bits->holding &= (1 << bits->length) - 1;

    if (n == 256)
    {
      printf("DONE\n");
      break;
    }

    if (n < 256)
    {
      if (n == '\n')
      {
        printf("%c", n);
      }
        else
      if (n >= ' ' && n <= 126)
      {
        printf("%c", n);
      }
        else
      {
        printf("<%02x>", n);
      }
    }
      else
    {
      int length_code = n - 257;
      int extra_bits = deflate_length_extra_bits[length_code];
      int length = deflate_length_codes[length_code];
      int data;

      data = 0;

      if (extra_bits != 0)
      {
        if (bits->length < extra_bits)
        {
          if (read_bits(in, bits) == -1) { break; }
        }

        data = bits->holding >> (bits->length - extra_bits);
        data = data & ((1 << extra_bits) - 1);
        bits->length -= extra_bits;
        bits->holding &= (1 << bits->length) - 1;

        data = deflate_reverse[data] >> (8 - extra_bits);

        length += data;
      }

      data = 0;

      if (bits->length < 5)
      {
        if (read_bits(in, bits) == -1) { break; }
      }

#if 0
      int distance_code = bits->holding >> (bits->length - 5);
      distance_code = distance_code & 0x1f;

      bits->length -= 5;
#endif

      for (n = 0; n < hdist_count; n++)
      {
        if (distances[n].length == 0) { continue; }

        // Find value of next code in stream.
        while (bits->length < distances[n].length)
        {
          if (read_bits(in, bits) == -1) { break; }
        }

        if (bits->length < distances[n].length) { break; }

        const int code = bits->holding >> (bits->length - distances[n].length);

        if (code == distances[n].code) { break; }
      }

      if (n >= hdist_count)
      {
        printf("Error: distance code doesn't exist\n");
        break;
      }

      bits->length -= distances[n].length;
      bits->holding &= (1 << bits->length) - 1;

      int distance_code = n;
      int distance = deflate_distance_codes[distance_code];
      extra_bits = deflate_distance_extra_bits[distance_code];

      if (extra_bits != 0)
      {
        if (bits->length < extra_bits)
        {
          if (read_bits(in, bits) == -1) { break; }
        }

        data = bits->holding >> (bits->length - extra_bits);
        data = data & ((1 << extra_bits) - 1);
        bits->length -= extra_bits;
        bits->holding &= (1 << bits->length) - 1;

        if (extra_bits <= 8)
        {
          data = deflate_reverse[data] >> (8 - extra_bits);
        }
        else
        {
          data = deflate_reverse[data & 0xff] << 8 | deflate_reverse[data >> 8];
          data = data >> (16 - extra_bits);
        }

        distance += data;
      }

      // printf("   -- distance_code=%d distance=%d extra_bits=%d extra_data=%d\n", distance_code, distance - data, extra_bits, data);

      printf("  [ length=%d distance=%d ]\n", length, distance);
    }
  }

  return 0;
}

int main(int argc, char *argv[])
{
  FILE *in;
  struct _gzip_header gzip_header;
  uint8_t compression_type = 3;
  struct _bits bits;

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

    if (compression_type != 0)
    {
      compression_type ^= 3;
    }

    const char *type = "error";

    switch(compression_type)
    {
      case 0:
        type = "uncompressed";
        break;
      case 1:
        type = "fixed";
        break;
      case 2:
        type = "dynamic";
        break;
    }

    printf("       byte=%02x (%02x)\n",
      bits.holding,
      deflate_reverse[bits.holding]);
    printf("      final=%d\n", bfinal);
    printf("       type=%s (%d)\n", type, compression_type);

    //bits.holding >>= 3;
    bits.length -= 3;
  }
  while(0);

  if (compression_type == 0)
  {
    inflate_uncompressed(in);
  }
  else if (compression_type == 1)
  {
    inflate_fixed_huffman(in, &bits);
  }
  else if (compression_type == 2)
  {
    inflate_dynamic_huffman(in, &bits);
  }

  fclose(in);

  return 0;
}

