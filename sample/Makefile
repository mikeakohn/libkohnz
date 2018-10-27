
default:
	gcc -o sample sample.c -Wall -O3 -lkohnz -L.. -I../src

mac:
	gcc -o sample sample.c -Wall -O3 \
	  ../build/crc32.o \
	  ../build/deflate_codes.o \
	  ../src/kohnz.c \
	  -I../src

clean:
	@rm -f mikemike.txt mikemike.txt.gz sample
	@echo "Clean!"

