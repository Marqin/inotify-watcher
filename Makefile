CC = clang
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99

.PHONY: all clean

all : inotify-watcher

inotify-watcher : Makefile inotify-watcher.c
	$(CC) $(CFLAGS)  inotify-watcher.c -o inotify-watcher

clean:
	rm -f ./inotify-watcher
