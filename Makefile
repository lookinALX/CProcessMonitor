CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
TARGETS = coordinator server

all: $(TARGETS)

coordinator: coordinator.c worker.c
	$(CC) $(CFLAGS) -o coordinator coordinator.c worker.c

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
