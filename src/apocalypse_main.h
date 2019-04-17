// Standard libraries
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Apocalypse FS implementation
#include "global_macros/global_macros.h"
#include "global_types/global_types.h"
#include "apocalypse_functions/apocalypse_functions.h"

// Have to inclue after due to header definition
#include <fuse3/fuse.h>