CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -I. -arch arm64
LDFLAGS = -arch arm64

OBJS = main.o help_table.o label_table.o decoding.o decode_to_string_array.o decode_to_int_array.o

Test: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) Test
