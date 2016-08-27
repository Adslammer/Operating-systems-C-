/*! signali.c
 *
 * Obradjuje signale SIGUSR1, SIGUSR2 i SIGTERM
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "signali.h"

/*!
 * Registrira zadani signal - kad se on dogodi pozvati ce se zadana funkcija
 * \param sig	Broj signala koji se registrira
 * \param func	Funkcija koja ce signal obraditi
 * \param f_prije	Adresa za pohranu vrijednosti prethodne funkcije za
 *			signal, ako je zadana (nije NULL)
 * \returns 0, ako je registracija uspjela, 1 inace
 */
int registriraj_signal ( int sig, const void *func, void *f_prije )
{
	struct sigaction act, oact;

	act.sa_sigaction = func;
	sigemptyset ( &act.sa_mask );	/* koji se jos signali blokiraju u
					   funkciji obrade ovog prekida */
	act.sa_flags = SA_SIGINFO;	/* uz signal ide i dodatni opis */

	if ( sigaction ( sig, &act, &oact ) )	/* 'registriranje' signala */
	{
		perror ( "sigaction\n" );
		return 1;
	}

	if ( f_prije != NULL ) /* treba li vratiti prijasnju funkciju? */
	{
		if ( oact.sa_flags & SA_SIGINFO )
			f_prije = oact.sa_sigaction;
		else
			f_prije = oact.sa_handler;
	}

	return 0;
}

/*!
 * Privremeno blokiraj signal (dok se ne definira drukcije)
 * Ostvareno tako da se zadani signal dodaje u "masku" onih koji su vec
 * blokirani.
 * \param sig	Broj signala koji se blokira
 * \returns 0, ako je blokiranje uspjelo, -1 inace
 */
int blokiraj_signal ( int sig )
{
	sigset_t set;

	sigemptyset ( &set );
	sigaddset ( &set, sig );

	return sigprocmask ( SIG_BLOCK, &set, NULL );
}

/*!
 * Odblokiraj signal
 * Ostvareno tako da se zadani signal mice iz "maske" blokiranih signala
 * \param sig	Broj signala koji se blokira
 * \returns 0, ako je odblokiranje uspjelo, -1 inace
 */
int odblokiraj_signal ( int sig )
{
	sigset_t set;

	sigemptyset ( &set );
	sigaddset ( &set, sig );

	return sigprocmask ( SIG_UNBLOCK, &set, NULL );
}

/*!
 * Slanje signala (drugom) procesu
 * \param sig	Broj signala koji se salje
 * \param kome	Identifikacijski broj procesa kome se signal salje (PID)
 * \param param	Broj koji se salje uz signal (dodatno uz sam signal)
 * \returns 0, ako je slanje uspjelo, 1 inace
 */
int posalji_signal ( int sig, int kome, int param )
{
	union sigval sv;

	sv.sival_int = param;

	if ( sigqueue ( kome, sig, sv ) )
	{
		perror ( "sigqueue" );
		return 1;
	}

	return 0;
}
