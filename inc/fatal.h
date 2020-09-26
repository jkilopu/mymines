#ifndef __FATAL_H
#define __FATAL_H

#include <stdio.h>
#include <stdlib.h>

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )
#define SDL_FatalError(str, func) fprintf(stderr, "%s\n%s\n", str, func), exit(1)
#define SDL_Error(str, func) SDL_FatalError(str, func)

#endif
