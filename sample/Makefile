
OBJECTS= \
  ../build/crc32.o \
  ../build/deflate_codes.o \

default:
	gcc -o sample_00 sample_00.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_01a sample_01a.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_01b sample_01b.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_01c sample_01c.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_01d sample_01d.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o sample_02a sample_02a.c -Wall -O3 -lkohnz -L.. -I../src
	gcc -o build_json build_json.c -Wall -O3 -lkohnz -L.. -I../src

mac:
	gcc -o sample sample.c -Wall -O3 \
	  $(OBJECTS) \
	  ../src/kohnz.c \
	  -I../src

clean:
	@rm -f sample_00 sample_01a sample_01b sample_01c sample_01d
	@rm -f mikemike.txt mikemike.txt.gz mikemike.bin mikemike.bin.gz
	@echo "Clean!"

