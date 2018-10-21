DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 $(DEBUG)
VPATH=../src
OBJS=crc32.o

default: $(OBJS)
	$(CC) -o ../parse_gz ../src/parse_gz.c $(CFLAGS)
	$(CC) -o ../libkohnz.so ../src/kohnz.c -FPIC -shared $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

