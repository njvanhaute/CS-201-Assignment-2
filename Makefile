CC=gcc
CFLAGS=-Wall -Wextra -O3 -c
LDFLAGS=

SRCS=main.c bst.c cda.c gt.c integer.c options.c queue.c rbt.c scanner.c string.c
OBJS=$(SRCS:.c=.o)
EXEC=trees

all : $(EXEC) $(SRCS)

$(EXEC) : $(OBJS)
	$(CC) -O3 $(LDFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean test
clean :
	rm -f $(EXEC) $(OBJS)

test : $(EXEC)
	./$(EXEC) tests/shakespeare.txt tests/shakespeare.go2