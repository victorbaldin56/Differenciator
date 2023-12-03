//////////////////////////////////////////////////////////////////////////////
/// @file
///
/// @brief Copyright (c) Victor Baldin, 2023.
///
//////////////////////////////////////////////////////////////////////////////

#include "ddx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char *LoadFile(const char pathname[])
{
    assert(pathname);

    struct stat stbuf = {};
    if (stat(pathname, &stbuf) == -1) return NULL;

    FILE *input = fopen(pathname, "r");
    if (!input) return NULL;

    char *buffer = (char *)calloc((size_t)stbuf.st_size + 1, sizeof(*buffer));
    if (!buffer) return NULL;

    fread(buffer, (size_t)stbuf.st_size, 1, input);
    return buffer;
}


