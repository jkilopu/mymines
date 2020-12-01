#ifndef __FATAL_H
#define __FATAL_H

#include <stdio.h>
#include <stdlib.h>

#define Error(str) FatalError(str)
#define FatalError(str) fprintf(stderr, "%s\n", str), exit(1)
#define FatalErrorInFunc(LOCATION, MSG) fprintf(stderr, "%s: %s\n", LOCATION, MSG), exit(1)
#define SDL_FatalError(...) fprintf(stderr, __VA_ARGS__), exit(1)
#define SDL_Error(...) SDL_FatalError(__VA_ARGS__)

#define MALLOC_FAIL_MSG "Malloc Failed!!!"
#define CALLOC_FAIL_MSG "Calloc Failed!!!"

void *malloc_fatal(size_t size, const char *location);
void *calloc_fatal(size_t num, size_t size, const char *location);

#endif
