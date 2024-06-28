CC = gcc
CFLAGS = -Wall

SRCS = server client

OBJS = $(SRCS:.c=)

all: $(OBJS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(SRCS)

style:
	clang-format -style=Google -i *.c