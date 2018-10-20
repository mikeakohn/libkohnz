DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 $(DEBUG)
VPATH=../src
OBJS=

default: $(OBJS)
	$(CC) -o ../parse_gz ../src/parse_gz.c $(CFLAGS)

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

