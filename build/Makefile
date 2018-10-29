DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 -fPIC $(DEBUG)
VPATH=../src
OBJECTS=crc32.o deflate_codes.o

default: $(OBJECTS)
	$(CC) -o ../parse_gz ../src/parse_gz.c deflate_codes.o $(CFLAGS)
	$(CC) -o ../libkohnz.so ../src/kohnz.c -shared \
	  $(OBJECTS) \
	  $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

