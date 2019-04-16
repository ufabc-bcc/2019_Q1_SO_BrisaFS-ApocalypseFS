// Defines compatible FUSE version
#define FUSE_USE_VERSION 31

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// FUSE specific header
#include <fuse.h>

// Apocalypse FS implementation
#include "global_macros/global_macros.h"
#include "global_types/global_types.h"
#include "apocalypse_functions/apocalypse_functions.h"
