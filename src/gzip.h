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

#ifndef _GZIP
#define _GZIP

struct _gzip_header
{
  uint8_t magic[2];
  uint8_t compression;
  uint8_t flags;
  uint32_t timestamp;
  uint8_t compression_flags;
  uint8_t operating_system;
};

#endif

