#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kohnz.h"

int main(int argc, char *argv[])
{
  struct _kohnz *kohnz;
  uint8_t buffer[32];
  uint16_t literals_sorted[285] = { ',', '1', '2', '9' };
  uint16_t distances_sorted[32];
  int n;

  kohnz_init();

  kohnz = kohnz_open("test.txt.gz", "test.txt", NULL);

  if (kohnz == NULL)
  {
    printf("Couldn't open file for writing\n");
    return 0;
  }

  sprintf((char *)buffer, "129,129,129,129");

  const int length = sizeof(buffer) - 1;

  memset(literals_sorted, 0, sizeof(literals_sorted));
  memset(distances_sorted, 0, sizeof(distances_sorted));

  int code = 257;

  for (n = 4; n <= 4 + (285 - 257); n++)
  {
    literals_sorted[n] = code++;
  }

  for (n = 0; n <= 29; n++)
  {
    distances_sorted[n] = n;
  }

  kohnz_start_dynamic_block(kohnz, 1, literals_sorted, distances_sorted, sizeof(literals_sorted), sizeof(distances_sorted));
  kohnz_write_dynamic(kohnz, buffer, length);
  kohnz_build_crc32(kohnz, buffer, length);

  kohnz_end_dynamic_block(kohnz);
  kohnz_close(kohnz);

  return 0;
}

