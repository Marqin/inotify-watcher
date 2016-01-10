/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#pragma once

#include <stdint.h>

typedef struct path {
  uint8_t * path;
  int wd;
} path_t;

path_t new_path( const uint8_t * const );
void del_path( path_t * );
