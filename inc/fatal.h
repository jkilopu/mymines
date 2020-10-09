#ifndef __FATAL_H
#define __FATAL_H

#include <stdio.h>
#include <stdlib.h>

#define Error(str) FatalError(str)
#define FatalError(str) fprintf(stderr, "%s\n", str), exit(1)
#define SDL_FatalError(...) fprintf(stderr, __VA_ARGS__), exit(1)
#define SDL_Error(...) SDL_FatalError(__VA_ARGS__)

#endif
