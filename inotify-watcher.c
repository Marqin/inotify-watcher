/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

// doxygen
//! \file inotify-watcher.c

#ifndef __linux__
  #error "Your OS is not supported."
#endif


#include <stdio.h>        // printing
#include <unistd.h>       // read(), close()
#include <sys/inotify.h>  // inotify stuff
#include <sys/ioctl.h>    // ioctl()
#include <signal.h>       // signal() and signals

//! Helper macro used in printing event name.
#define EVENT_NAME_MACRO  ( name ? *name : event->name )
//! Stop flag for watcher.
#define SIGNALS_TO_CATCH_LEN 6

//! Array of signals to catch.
const int SIGNALS_TO_CATCH[SIGNALS_TO_CATCH_LEN] = { SIGINT, SIGTERM, SIGABRT, SIGSEGV, SIGILL, SIGQUIT };

//! Stop flag for watcher.
int run = 1;

//! Signal handler.
/*!
  Stops watcher when caught signal from SIGNALS_TO_CATCH.
  \param sig signal number.
*/
void sig_handler( int sig ) {
  for( int i = 0; i < SIGNALS_TO_CATCH_LEN; i++ ) {
    if( sig == SIGNALS_TO_CATCH[i] ) {
      run = 0;
      break;
    }
  }
}


//! Event printer.
/*!
  \param event caught inotify event.
  \param path watched path.
*/
void printEvent( const struct inotify_event * const event, const char * const path ) {
  const char * const * name;

  if( event->len == 0 ) {
    name = &path;
  } else {
    name = NULL;
  }

  if (event->mask & IN_ACCESS)        printf( "\"%s\" was accessed. ", EVENT_NAME_MACRO );
  if (event->mask & IN_MODIFY)        printf( "\"%s\" was modified. ", EVENT_NAME_MACRO );
  if (event->mask & IN_ATTRIB)        printf( "\"%s\" has metadata changed. ", EVENT_NAME_MACRO );
  if (event->mask & IN_CLOSE_WRITE)   printf( "\"%s\" was closed (read-only). ", EVENT_NAME_MACRO );
  if (event->mask & IN_CLOSE_NOWRITE) printf( "\"%s\" was closed (writeable). ", EVENT_NAME_MACRO );
  if (event->mask & IN_OPEN)          printf( "\"%s\" was opened. ", EVENT_NAME_MACRO );
  if (event->mask & IN_MOVED_FROM)    printf( "\"%s\" was moved from \"%s\". ", EVENT_NAME_MACRO, path );
  if (event->mask & IN_MOVED_TO)      printf( "\"%s\" was moved to \"%s\". ", EVENT_NAME_MACRO, path );
  if (event->mask & IN_CREATE)        printf( "\"%s\" was created. ", EVENT_NAME_MACRO );
  if (event->mask & IN_DELETE)        printf( "\"%s\" was deleted. ", EVENT_NAME_MACRO );
  if (event->mask & IN_DELETE_SELF)   printf( "\"%s\" (self) was deleted. ", EVENT_NAME_MACRO );
  if (event->mask & IN_MOVE_SELF)     printf( "\"%s\" (self) was moved. ", EVENT_NAME_MACRO );

  if (event->mask & IN_UNMOUNT)       printf( "\"%s\" filesystem was unmounted! ", EVENT_NAME_MACRO );
  if (event->mask & IN_Q_OVERFLOW)    printf( "EVENT QUEUE OVERFLOW! " );
  if (event->mask & IN_IGNORED)       printf( "\"%s\" was ignored. ", EVENT_NAME_MACRO );

  if (event->mask & IN_ISDIR)         printf( "\"%s\" is directory. ", EVENT_NAME_MACRO );

  printf("\n");
}


//! Event printer.
/*!
  \param path path to watch.
*/
int watch( const char * const path ) {

  printf( "Watching path \"%s\".\n", path );

  const int fd = inotify_init();
  if ( fd < 0 ) {
    perror( "inotify_init" );
    return 0;  // false
  }

  const int wd = inotify_add_watch( fd, path, IN_ALL_EVENTS );
  if( wd < 0 ) {
    perror( "inotify_add_watch" );
    return 0;  // false;
  }

  while( run ) {

    unsigned int length = 0;
    ioctl( fd, FIONREAD, &length );  // check if there are new events
    if( length <= 0 ) {
      continue;
    }

    char buffer[length];
    read( fd, buffer, length );

    for( unsigned int i = 0; i < length; ) {
      const struct inotify_event * const event = (struct inotify_event *) &(buffer[i]);

      if( event == NULL ) {
        break;
      }

      printEvent( event, path );
      i += sizeof( struct inotify_event ) + event->len;
    }
  }

  inotify_rm_watch( fd, wd );
  close( fd );
  return 1;  // true
}


//! Program's entry point.
/*!
  \param argc argument count.
  \param argv program's arguments.
*/
int main( int argc, char * argv[] ) {

  for( int i = 0; i < SIGNALS_TO_CATCH_LEN; i++ ) {
    signal( SIGNALS_TO_CATCH[i], sig_handler );
  }

  if( argc != 2 ) {
    printf( "USAGE ./inotify-watcher PATH\n" );
    return 1;
  }

  if( ! watch( argv[1] ) ) {
    return 1;
  }

  return 0;
}
