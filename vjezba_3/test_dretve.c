/*!
 * Dretve - test
 */

#include <stdio.h>
#include <unistd.h>
#include "dretve_i_procesi.h"

#define BR_DRETVI	4
#define BR_POSLOVA	10

void posao ( int id, void *pparam, int iparam )
{
	printf ( "Zapocinje posao %d, pparam = %p, iparam = %d\n",
		 id, pparam, iparam );

	sleep ( iparam );

	printf ( "*Zavrsava posao %d, pparam = %p, iparam = %d\n",
		 id, pparam, iparam );
}

int main ( int br_arg, char *arg[] )
{
	void *dretve, *poslovi[BR_POSLOVA];
	unsigned int i;

	dretve = stvori_radne_dretve ( BR_DRETVI );

	printf ( "Stvaram poslove...\n" );

	for ( i = 0; i < BR_POSLOVA; i++ )
		dodaj_posao ( dretve, posao, &poslovi[i], ( i % 5 + 3 ) );

	printf ( "Poslovi stvoreni, cekam kraj\n" );

	cekaj_dovrsetak_poslova ( dretve, 1 );

	printf ( "Poslovi gotovi!\n" );

	makni_radne_dretve ( dretve );

	return 0;
}
