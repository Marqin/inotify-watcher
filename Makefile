CC = clang
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

.PHONY: all clean

all : inotify-watcher

inotify-watcher : Makefile inotify-watcher.c structs.c structs.h
	$(CC) $(CFLAGS)  inotify-watcher.c structs.c -o inotify-watcher

clean:
	rm -f ./inotify-watcher
