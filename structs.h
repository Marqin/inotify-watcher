/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#pragma once

typedef struct path {
  char * path;
  int wd;
} path_t;

path_t new_path( const char * const );
