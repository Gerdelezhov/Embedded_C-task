CC = gcc
CFLAGS = -Wall

SRC_FILES = 1.c
EXECUTABLES = $(SRC_FILES:.c=)

all: $(EXECUTABLES)

%: %.c
	clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i $<
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)
