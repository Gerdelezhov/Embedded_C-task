CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude
LIBFLAG = -lcalc

SRCDIR = src
HEADDIR = include
SRCLIBDIR = $(SRCDIR)/lib
LIBDIR = lib
OBJDIR = obj
BINDIR = bin

LIBNAME = libcalc.so

MAIN = $(SRCDIR)/main/main.c
APP = $(BINDIR)/main

LIBSRC = add.c sub.c mul.c div.c
LIBOBJ = $(LIBSRC:%.c=$(OBJDIR)/%.o)
INCAPP = $(HEADDIR)/libcalc.h
LIBTESTSRC = $(SRCDIR)/main/test_libcalc.c
LIBTESTBIN = $(BINDIR)/calctest

all: DIR $(LIBTESTBIN) $(APP)

$(APP): $(MAIN) $(LIBDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) $(MAIN) -L$(LIBDIR) $(LIBFLAG) -o $@

$(LIBTESTBIN): $(LIBTESTSRC) $(LIBDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) $(LIBFLAG) -o $@

$(LIBDIR)/$(LIBNAME): $(LIBOBJ)
	$(CC) -shared $^ -o $@

$(OBJDIR)/%.o: $(SRCLIBDIR)/%.c $(INCAPP)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

DIR:
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

.PHONY: style
style:
	find . -type f -name '*.[ch]' | xargs clang-format -style="{BasedOnStyle: Google, IndentWidth: 4}" -i --verbose

.PHONY: run
run:
	LD_LIBRARY_PATH=$$(pwd)/$(LIBDIR):$$LD_LIBRARY_PATH ./$(APP)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)
	rm -rf $(LIBDIR)
	rm -rf $(BINDIR)
