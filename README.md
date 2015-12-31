# inotify-watcher

[![Build Status](https://travis-ci.org/Marqin/inotify-watcher.svg)](https://travis-ci.org/Marqin/inotify-watcher)

Simple inotify tool for Linux to watch selected directory for changes.

It will print all catchable inotify events ( file write, close etc. )
for selected directory. Recursive watching is in TODO.

## Compilation

```make```

## Usage

```./inotify-watcher PATH```
(example: ```./inotify-watcher /tmp```)

## TODO

* recursive watch
* dynamically watch/unwach added/removed dirs to recursive watch
