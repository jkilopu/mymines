/**
 * @file fatal.c
 * @author jkilopu
 * @brief Provide wrapped error handle functions.
 */
#include "fatal.h"
#include <stdlib.h>

#define FatalErrorInFunc(LOCATION, MSG) Error("%s: %s\n", LOCATION, MSG)

/**
 * @brief Wrapper fuction for "malloc".
 * 
 * @param size malloc(size)
 * @param location The traceback.
 */
void *malloc_fatal(size_t size, const char *location)
{
    void *malloced_addr = malloc(size);
    if (malloced_addr == NULL)
        FatalErrorInFunc(location, MALLOC_FAIL_MSG);
    return malloced_addr;
}

/**
 * @brief Wrapper fuction for "calloc".
 * 
 * @param num calloc(num, )
 * @param size calloc(, size)
 * @param location The traceback.
 */
void *calloc_fatal(size_t num, size_t size, const char *location)
{
    void *calloced_addr = calloc(num, size);
    if (calloced_addr == NULL)
        FatalErrorInFunc(location, CALLOC_FAIL_MSG);
    return calloced_addr;
}
