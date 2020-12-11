/**
 * @file fatal.h
 * @author jkilopu
 * @brief Provide error handle methods which can simplify error handling.
 */

#ifndef __FATAL_H
#define __FATAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//-------------------------------------------------------------------
// Error Handle Macros
//-------------------------------------------------------------------

#define Error(...) fprintf(output, __VA_ARGS__), exit(1)
#define FatalError(...) Error(__VA_ARGS__)
#define SDL_output_error(...) Error(__VA_ARGS__)
#define SDL_output_fatal_error(...) SDL_output_error(__VA_ARGS__)

//-------------------------------------------------------------------
// Error Message Macros
//-------------------------------------------------------------------

#define MALLOC_FAIL_MSG "Malloc Failed!!!"
#define CALLOC_FAIL_MSG "Calloc Failed!!!"

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void set_fatal_output_stream(bool is_error_log_file);
void *malloc_fatal(size_t size, const char *location);
void *calloc_fatal(size_t num, size_t size, const char *location);
void close_fatal_output_stream(void);

#endif
