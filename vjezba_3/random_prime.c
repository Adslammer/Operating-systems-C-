#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
          
int random_prime ()
{
	static unsigned int seed = -1;
	unsigned int i, a, x, max_q;
	
	if ( seed == -1 )
		seed = time (NULL) + getpid();

	while ( ( x = rand_r ( &seed ) ) < 10 )
		;

	x |= 1; /* neka x bude neparan */

	max_q = (int) sqrt ( (double) x );

	for ( i = x; ; i += 2 )
	{
		for ( a = 3; a <= max_q; a += 2 )
			if ( (i % a) == 0 )
				break;
		if ( a > max_q )
			break; /* 'i' is prosti broj */
	}

	return i;
}

/* Koristenje funkcije iz druge datoteke, npr. lab3a1.c:
 *
 * U lab3a1.c negdje ispod 'include'-ova dodat:
 *   extern int random_prime();
 *
 * Prevodjenje:
 *  gcc lab3a1.c random_prime.c -lm -o lab3a1
 *
 * Pokretanje:
 *  ./lab3a1 5 10
 */
