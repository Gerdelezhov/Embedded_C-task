SERVER_BIN := server
CLIENT_BIN := client
HEADERS := *.h
SOURCES := *.c
CC := gcc
CFLAGS := -g
OFLAGS := -c
LIBS := -lcurses

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): server.c
	$(CC) -o $(SERVER_BIN) server.c $(CFLAGS) $(LIBS)
$(SERVER_BIN): $(HEADERS)

$(CLIENT_BIN): client.c
	$(CC) -o $(CLIENT_BIN) client.c $(CFLAGS) $(LIBS)
$(CLIENT_BIN): $(HEADERS)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN) *.o

style:
	clang-format -style=Google -i *.c *.h
