#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "kohnz.h"

struct _entry
{
  const uint8_t *name;
  char value_string[32];
  int name_length;
  int value_length;
  int value;
  int new_value;
  uint32_t offset;
};

// 0: airspeed
// 1: heading
// 2: altitude
// 3: count
// 4: errors

const char *names[] =
{
  "    \"airspeed\": ",
  "    \"heading\": ",
  "    \"altitude\": ",
  "    \"count\": ",
  "    \"errors\": ",
};

int init_entries(struct _entry *entries)
{
  int n;

  memset(entries, 0, sizeof(struct _entry) * 5);

  entries[0].value = (rand() % 4) + 300;
  entries[1].value = (rand() % 4) + 180;
  entries[2].value = (rand() % 200) + 35000;
  entries[3].value = 0;
  entries[4].value = 0;

  for (n = 0; n < 5; n++)
  {
    entries[n].name = (uint8_t *)names[n];
    entries[n].name_length = strlen(names[n]);
  }

  return 0; 
}

int update_entries(struct _entry *entries)
{
  entries[0].new_value = (rand() % 4) + 300;
  entries[1].new_value = (rand() % 4) + 180;
  entries[2].new_value = (rand() % 200) + 35000;
  entries[3].new_value++;
  entries[4].new_value = 0;

  return 0; 
}

int add_entry(struct _kohnz *kohnz, struct _entry *entries, int last)
{
  int n;

  kohnz_write_fixed(kohnz, (const uint8_t *)"  {\n", 4);
  kohnz_build_crc32(kohnz, (const uint8_t *)"  {\n", 4);

  if (entries[0].offset == 0)
  {
    // First time entries are logged, so don't bother with lz77.
    for (n = 0; n < 5; n++)
    {
      entries[n].offset = kohnz_get_offset(kohnz);
      kohnz_write_fixed(kohnz,  entries[n].name, entries[n].name_length);
      kohnz_build_crc32(kohnz,  entries[n].name, entries[n].name_length);

      if (n < 4)
      {
        sprintf(entries[n].value_string, "%d,\n", entries[n].value);
      }
        else
      {
        sprintf(entries[n].value_string, "%d\n", entries[n].value);
      }

      entries[n].value_length = strlen(entries[n].value_string);

      kohnz_write_fixed(kohnz, (uint8_t *)entries[n].value_string, entries[n].value_length);
      kohnz_build_crc32(kohnz, (uint8_t *)entries[n].value_string, entries[n].value_length);
    }
  }
    else
  {
    update_entries(entries);

    for (n = 0; n < 5; n++)
    {
      uint64_t last_offset = entries[n].offset;

      entries[n].offset = kohnz_get_offset(kohnz);

      int distance = entries[n].offset - last_offset;

      if (entries[n].value != entries[n].new_value)
      {
        entries[n].value = entries[n].new_value;

        const int length =  entries[n].name_length;
        kohnz_write_fixed_lz77(kohnz, distance, length);
        kohnz_build_crc32(kohnz,  entries[n].name, length);

        if (n < 4)
        {
          sprintf(entries[n].value_string, "%d,\n", entries[n].value);
        }
          else
        {
          sprintf(entries[n].value_string, "%d\n", entries[n].value);
        }

        entries[n].value_length = strlen(entries[n].value_string);

        const uint8_t *value = (const uint8_t *)entries[n].value_string;
        kohnz_write_fixed(kohnz, value, entries[n].value_length);
        kohnz_build_crc32(kohnz, value, entries[n].value_length);
      }
        else
      {
        const int length =  entries[n].name_length +  entries[n].value_length;
        kohnz_write_fixed_lz77(kohnz, distance, length);
        kohnz_build_crc32(kohnz, entries[n].name, entries[n].name_length);
        kohnz_build_crc32(kohnz, (uint8_t *)entries[n].value_string, entries[n].value_length);
      }
    }
  }

  if (last == 0)
  {
    kohnz_write_fixed(kohnz, (const uint8_t *)"  },\n", 5);
    kohnz_build_crc32(kohnz, (const uint8_t *)"  },\n", 5);
  }
    else
  {
    kohnz_write_fixed(kohnz, (const uint8_t *)"  }\n", 4);
    kohnz_build_crc32(kohnz, (const uint8_t *)"  }\n", 4);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  struct _kohnz *kohnz;
  struct _entry entries[5];
  int n;

  srand(time(NULL));
  init_entries(entries);

  kohnz_init();

  kohnz = kohnz_open("airplane.json.gz", "airplane.json", NULL);

  if (kohnz == NULL)
  {
    printf("Couldn't open file for writing\n");
    return 0;
  }

  kohnz_start_fixed_block(kohnz, 1);

  kohnz_write_fixed(kohnz, (const uint8_t *)"[\n", 2);
  kohnz_build_crc32(kohnz, (const uint8_t *)"[\n", 2);

  for (n = 0; n < 1000; n++)
  {
    add_entry(kohnz, entries, n == 999);
  }

  kohnz_write_fixed(kohnz, (const uint8_t *)"]\n", 2);
  kohnz_build_crc32(kohnz, (const uint8_t *)"]\n", 2);
  kohnz_end_fixed_block(kohnz);
  kohnz_close(kohnz);

  return 0;
}

