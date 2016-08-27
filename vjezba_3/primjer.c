/*! Primjer sinkronizacije
    Dretva 'kontrola' povećava globalnu varijablu 'broj' i pri svakom
    povećanju propušta dretve 'posao_n' koje tada mogu nešto napraviti */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define POSLOVA 10
#define DO_BROJA 20

/* globalne varijable = zajednicke varijable */
int broj, obradjeno;
pthread_mutex_t monitor;
pthread_cond_t red_poslova, red_kontrola;

/*! dretve 'poslova'; 'aktiviraju' se na visekratnik od 'n' */
void *posao_n ( void *x )
{
	int n;

	n = (int) x;

	pthread_mutex_lock ( &monitor );

	obradjeno++;
	if ( obradjeno == POSLOVA )
		pthread_cond_signal ( &red_kontrola );

	while ( broj < DO_BROJA )
	{
		pthread_cond_wait ( &red_poslova, &monitor );

		if ( broj % n == 0 )
			printf ( "Aktiviram posao %d\n", n );

		obradjeno++;

		if ( obradjeno == POSLOVA )
			pthread_cond_signal ( &red_kontrola );
	}

	pthread_mutex_unlock ( &monitor );

	return NULL;
}

/*! dretva kontrola, povecava broj kad su svi poslovi prosli */
void *kontrola ( void *x )
{
	pthread_mutex_lock ( &monitor );

	while ( broj < DO_BROJA )
	{
		while ( obradjeno < POSLOVA && broj < DO_BROJA )
			pthread_cond_wait ( &red_kontrola, &monitor );

		broj++;
		obradjeno = 0;

		printf ( "Obrada broja %d\n", broj );

		pthread_cond_broadcast ( &red_poslova );
	}

	pthread_mutex_unlock ( &monitor );

	return NULL;
}

int main ()
{
	int i;
	pthread_t dretve[1 + POSLOVA];

	broj = 0;
	obradjeno = 0;

	pthread_mutex_init ( &monitor, NULL );
	pthread_cond_init ( &red_poslova, NULL );
	pthread_cond_init ( &red_kontrola, NULL );

	if ( pthread_create ( &dretve[0], NULL, kontrola, NULL ) )
	{
		fprintf ( stderr, "Greska pri stvaranju dretve!\n" );
		exit (1);
	}

	for ( i = 1; i <= POSLOVA; i++ )
	{
		if ( pthread_create ( &dretve[i], NULL, posao_n, (void *) i ) )
		{
			fprintf ( stderr, "Greska pri stvaranju dretve!\n" );
			exit (1);
		}
	}

	for ( i = 0; i <= POSLOVA; i++ )
		pthread_join ( dretve[i], NULL ); /* ceka zavrsetak dretve */

	return 0;
}
