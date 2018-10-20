#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
  FILE *in;

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

  fclose(in);

  return 0;
}

