/**
 * @file prng_lcg.c
 * @author jkilopu
 * @brief Use "Linear Congruential Generator" to generate cross platform and consistent pseudo-random numbers.
 * 
 * @note Understand the algorithm:
 * 1. Basic knowledge: https://zhuanlan.zhihu.com/p/96666921
 * 2. Why the algorithm works: https://zhuanlan.zhihu.com/p/36301602
 * 3. Glibc implementation: https://stackoverflow.com/questions/18634079/glibc-rand-function-implementation
 *                          https://github.com/bminor/glibc/blob/master/stdlib/random_r.c
 * 4. Shortage: https://www.vincent-lunot.com/post/playing-with-pseudo-random-number-generators-part-2/
 * 5. Truncated lcg and Lattice based cryptography: https://crypto.stackexchange.com/questions/37836/problem-with-lll-reduction-on-truncated-lcg-schemes
 * 6. Attack: http://www.mamicode.com/info-detail-2810905.html
 */

#include "prng_lcg.h"
#include <stdint.h>
#include <stdio.h>

static uint32_t nSeed;

/**
 * @brief Equivalent to "srand" in standard lib.
 * 
 * @param seed The seed.
 */
void prng_lcg_srand(uint32_t seed)
{
    nSeed = seed;
}

/**
 * @brief Equivalent to "rand" in standard lib.
 * 
 * @return A Number in [0, 0x7fffffff].
 */
int32_t prng_lcg_rand(void)
{
    /* Take the current seed and generate a new value from it.
     * Due to our use of large constants and overflow, it would be
     * very hard for someone to predict what the next number is
     * going to be from the previous one.
     * 
     * Values are from glibc.
     * It seems that M can't be 0xffffffff (max unsigend int)
     * for reasons listed at the top of file (and maybe error handling need in glibc?).
     */    
    nSeed = (1103515245U * nSeed + 12345U) % 0x7fffffff; ///< Note that the highest bit of "nSeed" is 0,
                                                         ///< becasue of "& 0x7fffffff
    
    /* Take the seed and return a [0, 0x7fffffff] */
    return nSeed;
}
