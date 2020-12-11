#ifndef PRNG_H_INCLUDED
#define PRNG_H_INCLUDED

#include <stddef.h>

void prng_rc4_seed_time (void);
void prng_rc4_seed_bytes (const void *, size_t);
unsigned char prng_rc4_get_octet (void);
unsigned char prng_rc4_get_byte (void);
void prng_rc4_get_bytes (void *, size_t);
unsigned long prng_rc4_get_ulong (void);
long prng_rc4_get_long (void);
unsigned prng_rc4_get_uint (void);
int prng_rc4_get_int (void);
double prng_rc4_get_double (void);
double prng_rc4_get_double_normal (void);

#endif /* prng.h */
