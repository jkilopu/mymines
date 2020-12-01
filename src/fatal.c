#include "fatal.h"
#include <stdlib.h>

void *malloc_fatal(size_t size, const char *location)
{
    void *malloced_addr = malloc(size);
    if (malloced_addr == NULL)
        FatalErrorInFunc(location, MALLOC_FAIL_MSG);
    return malloced_addr;
}

void *calloc_fatal(size_t num, size_t size, const char *location)
{
    void *calloced_addr = calloc(num, size);
    if (calloced_addr == NULL)
        FatalErrorInFunc(location, CALLOC_FAIL_MSG);
    return calloced_addr;
}