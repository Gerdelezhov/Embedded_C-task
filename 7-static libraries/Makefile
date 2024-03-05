CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude
LIBFLAG = -lcalc

SRCDIR = src
HEADDIR = include
SRCLIBDIR = $(SRCDIR)/lib
LIBDIR = lib
OBJDIR = obj
BINDIR = bin

LIBNAME = libcalc.a

MAIN = $(SRCDIR)/main/main.c
APP = $(BINDIR)/main

LIBSRC = add.c sub.c mul.c div.c
LIBOBJ = $(LIBSRC:%.c=$(OBJDIR)/%.o)
INCAPP = $(HEADDIR)/libcalc.h
LIBTESTSRC = $(SRCDIR)/main/test_libcalc.c
LIBTESTBIN = $(BINDIR)/calc

all: DIR $(LIBTESTBIN) $(APP)

# Main executable file
$(APP): $(MAIN) $(LIBDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) $(MAIN) -L$(LIBDIR) $(LIBFLAG) -o $@

# Test executable file for the library
$(LIBTESTBIN): $(LIBTESTSRC) $(LIBDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) $(LIBFLAG) -o $@

# Library
$(LIBDIR)/$(LIBNAME): $(LIBOBJ)
	ar rcs $@ $^

# Pattern rule to compile source files into object files
$(OBJDIR)/%.o: $(SRCLIBDIR)/%.c $(INCAPP)
	$(CC) $(CFLAGS) -c $< -o $@

DIR:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

.PHONY: style
style:
	find . -type f -name '*.[ch]' | xargs clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i --verbose

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)
	rm -rf $(BINDIR)
