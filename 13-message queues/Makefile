CC = gcc
CFLAGS = -Wall

SRC_FILES = 1-posix/client.c 1-posix/server.c
EXECUTABLES = $(SRC_FILES:.c=)

all: $(EXECUTABLES)

%: %.c
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i $<
	$(CC) $(CFLAGS) $< -o $@ -lrt

clean:
	rm -f $(EXECUTABLES)
