#ifndef __FATAL_H
#define __FATAL_H

#include <stdio.h>
#include <stdlib.h>

#define Error(str) FatalError(str)
#define FatalError(str) fprintf(stderr, "%s\n", str), exit(1)
#define SDL_FatalError(str, func) fprintf(stderr, "%s\n%s\n", str, func), exit(1)
#define SDL_Error(str, func) SDL_FatalError(str, func)

#endif
