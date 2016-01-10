/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#include <stdlib.h>
#include <string.h>
#include "structs.h"

path_t new_path( const uint8_t * const watch_path ) {
  path_t p;
  p.path = calloc( strlen((char*)watch_path) + 1, sizeof(uint8_t) );
  strcpy( (char*)p.path, (char*)watch_path );
  p.wd = -1;
  return p;
}

void del_path( path_t * p ) {
  if( p->path != NULL ) {
    free( p->path );
  }
}
