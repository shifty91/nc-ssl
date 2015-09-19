#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

void *kurt_malloc(size_t size)
{
    void *mem = malloc(size);
    if (!mem)
        err("malloc() failed: %s", strerror(errno));
    return mem;
}

void kurt_free(void **ptr)
{
    free(*ptr);
    *ptr = NULL;
}
