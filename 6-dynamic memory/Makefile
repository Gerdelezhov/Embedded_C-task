TARGET = app

CC = gcc
CFLAGS = -Wall -Wextra -pedantic

SRCS = main.c directory.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean style

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

style:
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i *.c *.h