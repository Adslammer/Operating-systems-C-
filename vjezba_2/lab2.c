/*! lab2.c
 *
 * Obradjuje signale SIGUSR1, SIGUSR2 i SIGTERM
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "signali.h"

/*!
 * Primjer funkcije za obradu signala
 * \param sig	Broj signala koji je primljen
 * \param info	Dodatne informacije o signalu (ako nije NULL)
 * \param context	ignorirati :) (ili pogledati man stranice za sigaction)
 */
void obrada ( int sig, siginfo_t *info, void *context )
{
	printf ( "Primio signal %d", sig );

	if ( info != NULL && info->si_code == SI_QUEUE )
		printf ( " uz parametar = %d (%p)\n", info->si_value.sival_int,
			 info->si_value.sival_ptr );
	else
		printf ( "\n" );
}

int main ()
{
	printf ( "Proces %ld poceo\n", (long) getpid() );

	registriraj_signal ( SIGTERM, obrada, NULL );
	registriraj_signal ( SIGUSR1, obrada, NULL );
	registriraj_signal ( SIGUSR2, obrada, NULL );

	for (;;)
		sleep (1);

	return 0;
}

