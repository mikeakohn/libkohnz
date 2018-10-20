DEBUG=-DDEBUG -g
CFLAGS=-Wall -O3 $(DEBUG) $(INCLUDES)
VPATH=../src
OBJS=

default: $(OBJS)
	$(CC) -o ../parse_gz ../src/parse_gz.c

%.o: %.c %.h
	$(CC) -c $< -o $*.o $(CFLAGS)

