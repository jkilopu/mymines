/**
 * @file fatal.h
 * @author jkilopu
 * @brief Provide error handle methods which can simplify error handling.
 */

#ifndef __FATAL_H
#define __FATAL_H

#include "SDL_Log.h"
#include <stdio.h>
#include <stdlib.h>

enum {
    SDL_LOG_CATEGORY_NET = SDL_LOG_CATEGORY_CUSTOM,
};

//-------------------------------------------------------------------
// Error Handle Macros
//-------------------------------------------------------------------

#define Error(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__), exit(1)
#define FatalError(...) Error(__VA_ARGS__)
#define SDL_other_fatal_error(...) Error(__VA_ARGS__)
#define SDL_net_error(...) SDL_LogError(SDL_LOG_CATEGORY_NET, __VA_ARGS__), exit(1)
#define SDL_render_fatal_error(...) SDL_LogError(SDL_LOG_CATEGORY_RENDER, __VA_ARGS__), exit(1)

//-------------------------------------------------------------------
// Error Message Macros
//-------------------------------------------------------------------

#define MALLOC_FAIL_MSG "Malloc Failed!!!"
#define CALLOC_FAIL_MSG "Calloc Failed!!!"

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void *malloc_fatal(size_t size, const char *location);
void *calloc_fatal(size_t num, size_t size, const char *location);

#endif
