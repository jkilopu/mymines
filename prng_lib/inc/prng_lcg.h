/**
 * @file prng_lcg.h
 * @author jkilopu
 * @brief A cross platform pseudorandom number generator.
 * 
 * @note Modified from:
 * https://stackoverflow.com/questions/922358/consistent-pseudo-random-numbers-across-platforms
 */

#ifndef __PRNG_H_
#define __PRNG_H_

#include <stdint.h>

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void prng_lcg_srand(uint32_t seed);
int32_t prng_lcg_rand(void);

#endif
