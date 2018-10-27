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

int deflate_length_codes[29] =
{
    3,   4,   5,   6,   7,  8,  9,  10,
   11,  13,  15,  17,  19, 23, 27,  31,
   35,  43,  51,  59,  67, 83, 99, 115,
  131, 163, 195, 227, 258
};

int deflate_length_extra_bits[29] =
{
  0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 2, 2, 2, 2,
  3, 3, 3, 3, 4, 4, 4, 4,
  5, 5, 5, 5, 0
};

int deflate_distance_codes[30] =
{
     1,    2,    3,     4,     5,     7,    9,   13,
    17,   25,   33,    49,    65,    97,  129,  193,
   257,  385,  513,   769,  1025,  1537, 2049, 3073,
  4097, 6145, 8193, 12289, 16385, 24577
};

int deflate_dist_extra_bits[30] =
{
   0,  0,  0,  0,  1,  1,  2,  2,
   3,  3,  4,  4,  5,  5,  6,  6,
   7,  7,  8,  8,  9,  9, 10, 10,
  11, 11, 12, 12, 13, 13
};

