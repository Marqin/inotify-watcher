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
#include <stdlib.h>       // calloc, free
#include <string.h>       // strlen, strcat, strcpy
#include <stdint.h>
#include <stdbool.h>
#include "structs.h"

//! Stop flag for watcher.
#define SIGNALS_TO_CATCH_LEN 6

//! Array of signals to catch.
const uint8_t SIGNALS_TO_CATCH[SIGNALS_TO_CATCH_LEN] = { SIGINT, SIGTERM, SIGABRT, SIGSEGV, SIGILL, SIGQUIT };

//! Stop flag for watcher.
bool run = true;

//! Signal handler.
/*!
  Stops watcher when caught signal from SIGNALS_TO_CATCH.
  \param sig signal number.
*/
void sig_handler( int sig ) {
  for( uint8_t i = 0; i < SIGNALS_TO_CATCH_LEN; i++ ) {
    if( sig == SIGNALS_TO_CATCH[i] ) {
      run = false;
      break;
    }
  }
}


//! Event printer.
/*!
  \param event caught inotify event.
  \param path watched path.
*/
void printEvent( const struct inotify_event * const event, const path_t p ) {
  uint8_t * name;

  if( event->len == 0 ) {
    name = calloc( strlen((char *)p.path) + 1, sizeof(uint8_t) );
    strcpy( (char *)name, (char *)p.path );
  } else {
    name = calloc( strlen((char *)event->name) + strlen((char *)p.path) + 2, sizeof(uint8_t) );
    strcpy( (char *)name, (char *)p.path );
    if( strlen((char *)name) > 0 && name[strlen((char *)name) - 1] != '/' ) {
      strcat( (char *)name, "/" );
    }
    strcat( (char *)name, (char *)event->name );
  }

  if( event->mask & IN_ACCESS )        printf( "\"%s\" was accessed. ", name );
  if( event->mask & IN_MODIFY )        printf( "\"%s\" was modified. ", name );
  if( event->mask & IN_ATTRIB )        printf( "\"%s\" has metadata changed. ", name );
  if( event->mask & IN_CLOSE_WRITE )   printf( "\"%s\" was closed (read-only). ", name );
  if( event->mask & IN_CLOSE_NOWRITE ) printf( "\"%s\" was closed (writeable). ", name );
  if( event->mask & IN_OPEN )          printf( "\"%s\" was opened. ", name );
  if( event->mask & IN_MOVED_FROM )    printf( "\"%s\" was moved from \"%s\". ", name, p.path );
  if( event->mask & IN_MOVED_TO )      printf( "\"%s\" was moved to \"%s\". ", name, p.path );
  if( event->mask & IN_CREATE )        printf( "\"%s\" was created. ", name );
  if( event->mask & IN_DELETE )        printf( "\"%s\" was deleted. ", name );
  if( event->mask & IN_DELETE_SELF )   printf( "\"%s\" (self) was deleted. ", name );
  if( event->mask & IN_MOVE_SELF )     printf( "\"%s\" (self) was moved. ", name );

  if( event->mask & IN_UNMOUNT )       printf( "\"%s\" filesystem was unmounted! ", name );
  if( event->mask & IN_Q_OVERFLOW )    printf( "EVENT QUEUE OVERFLOW! " );
  if( event->mask & IN_IGNORED )       printf( "\"%s\" was ignored. ", name );
  // ignored is send after path was removed from inotify

  if( event->mask & IN_ISDIR )         printf( "\"%s\" is directory. ", name );

  free(name);

  printf("\n");
}


//! Event printer.
/*!
  \param path path to watch.
*/
bool watch( const uint8_t * const watch_path ) {

  path_t p = new_path( watch_path );

  printf( "Watching path \"%s\".\n", p.path );

  const int fd = inotify_init();
  if ( fd < 0 ) {
    perror( "inotify_init" );
    return false;
  }

  p.wd = inotify_add_watch( fd, (char *)p.path, IN_ALL_EVENTS );
  if( p.wd < 0 ) {
    perror( "inotify_add_watch" );
    return false;
  }

  while( run ) {

    size_t length = 0;
    ioctl( fd, FIONREAD, &length );  // check if there are new events
    if( length <= 0 ) {
      continue;
    }

    uint8_t * buffer = calloc( length, sizeof(uint8_t *) );
    read( fd, buffer, length );

    for( size_t i = 0; i < length; ) {
      const struct inotify_event * const event = (struct inotify_event *) &(buffer[i]);

      if( event == NULL ) {
        break;
      }

      printEvent( event, p );
      i += sizeof( struct inotify_event ) + event->len;
    }
  }

  del_path( &p );
  inotify_rm_watch( fd, p.wd );
  close( fd );
  return true;
}


//! Program's entry point.
/*!
  \param argc argument count.
  \param argv program's arguments.
*/
int main( int argc, char * argv[] ) {

  for( uint8_t i = 0; i < SIGNALS_TO_CATCH_LEN; i++ ) {
    signal( SIGNALS_TO_CATCH[i], sig_handler );
  }

  if( argc != 2 ) {
    printf( "USAGE ./inotify-watcher PATH\n" );
    return 1;
  }

  if( ! watch( (uint8_t *)(argv[1]) ) ) {
    return 1;
  }

  return 0;
}
