CC = gcc
CFLAGS = -Wall -Werror

SRC_FILES = client.c server.c
EXECUTABLES = $(SRC_FILES:.c=)

all: $(EXECUTABLES)

%: %.c
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i $<
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)
