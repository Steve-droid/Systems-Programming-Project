CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=
INCLUDES=-I.
SRCS=main.c vector.c
OBJS=$(SRCS:.c=.o)
TARGET=vector

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)