DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 $(DEBUG)
VPATH=../src
OBJECTS=crc32.o deflate_codes.o

default: $(OBJECTS)
	$(CC) -o ../parse_gz ../src/parse_gz.c $(CFLAGS)
	$(CC) -o ../libkohnz.so ../src/kohnz.c -FPIC -shared \
	  $(OBJECTS) \
	  $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

