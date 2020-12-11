/**
 * @file fatal.c
 * @author jkilopu
 * @brief Provide wrapped error handle functions.
 */
#include "fatal.h"
#include <stdlib.h>
#include <stdbool.h>

#define FatalErrorInFunc(LOCATION, MSG) Error("%s: %s\n", LOCATION, MSG)

FILE *output;

/**
 * @brief Set the fatal output stream to "stderr" of a file
 * 
 * @param is_error_log_file Is the output stream point to a file.
 */
void set_fatal_output_stream(bool is_error_log_file)
{
    if (is_error_log_file)
    {
        output = fopen("mymines_error.txt", "w");
        if (output == NULL)
            exit(1);
    }
    else
        output = stderr;
}

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

/**
 * @brief Close the fatal output stream if it points to a file.
 */
void close_fatal_output_stream(void)
{
    if (output != stderr)
        if (fclose(output) < 0)
        {
            fprintf(stderr, "Close output failed.\n");
            exit(1);
        }
}
