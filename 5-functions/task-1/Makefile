TARGET = app

CC = gcc
CFLAGS = -Wall

SRCS = main.c directory.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i *.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
