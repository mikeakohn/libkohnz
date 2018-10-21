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

#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>

int kohnz_crc32_init();
uint32_t kohnz_crc32(uint8_t *buffer, int len, uint32_t crc);

#endif

