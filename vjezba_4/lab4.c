/*! zakljucaj.c */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "brave.h"

int main ( int argc, char *argv[] )
{
	int naredba, cekaj, vrati;
	void *zaklj;

	if ( argc < 2 )
	{
		kako_se_koristi:
		printf ( "Uporaba: %s naredba [objekt_za_zaklj.]\n", argv[0] );
		printf ( "naredbe: c (citaj), i (ne mjenjaj ime), "
			"m (mjenjaj samo sadrzaj),\n\te (ekskluzivno), "
			"p (prikazi sve brave), b (obrisi sve)\n"
			"\tdrugo slovo (uz 'cime'): b (blokiraj), t (test)\n");
		return 1;
	}

	naredba = 0;
	vrati = 0;

	switch ( argv[1][0] )
	{
		case 'c':
			naredba = SAMOCITAJ;
			break;

		case 'i':
			naredba = NEMJIME;
			break;

		case 'm':
			naredba = MJSADRZAJ;
			break;

		case 'e':
			naredba = EKSKLUZIV;
			break;

		case 'p':
			ispisi_brave ();
			break;

		case 'b':
			brisi_sve_brave ();
			break;

		default:
			goto kako_se_koristi;
	}

	if ( naredba )
	{
		if ( argc < 3 )
			goto kako_se_koristi;

		cekaj = ( argv[1][1] == 'b' );

		zaklj = zakljucaj ( argv[2], naredba, cekaj );

		vrati = ! zaklj;

		if ( vrati )
		{
			printf ( "Zakljucavanje nije uspjelo\n" );
		}
		else if ( argv[1][1] == 't' || argv[1][2] == 't' )
		{
			//ispisi_brave ();
			sleep (3);
			otkljucaj ( zaklj );
			//ispisi_brave ();
		}
	}

	return vrati;
}
