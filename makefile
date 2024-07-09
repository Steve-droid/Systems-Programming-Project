CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I. -arch arm64
LDFLAGS = -arch arm64

OBJS = main.o make_label_table.o decoding.o

Test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) Test
