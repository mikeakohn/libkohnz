
OBJECTS= \
  ../build/crc32.o \
  ../build/deflate_codes.o \

default:
	gcc -o sample_00 sample_00.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_01 sample_01.c -Wall -O3 -lkohnz -L.. -I../src

mac:
	gcc -o sample sample.c -Wall -O3 \
	  $(OBJECTS) \
	  ../src/kohnz.c \
	  -I../src

clean:
	@rm -f mikemike.txt mikemike.txt.gz sample_00 sample_01
	@echo "Clean!"

