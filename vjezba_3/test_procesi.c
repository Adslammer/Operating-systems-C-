/*!
 * Procesi - test
 */

#include <stdio.h>
#include <unistd.h>
#include "dretve_i_procesi.h"

void test ( void *pparam, int iparam )
{
	printf ( "*Proces dijete %d, pparam = %p, iparam = %d\n",
		 (int) getpid(), pparam, iparam );

	sleep ( 1 );

	printf ( "#Proces dijete %d, pparam = %p, iparam = %d\n",
		 (int) getpid(), pparam, iparam );
}

int main ( int br_arg, char *arg[] )
{
	pid_t pid;
	int status;

	printf ( ">>> Stvaranje procesa sa: "
		 "'stvori_proces_dijete ( test, &pid, 777 )' >>>\n" );

	pid = stvori_proces_dijete ( test, &pid, 777, 0 );

	printf ( ">>> Stvoren proces %d, param = %p\n", (int) pid, &pid );

	status = cekaj_kraj_procesa ( pid );

	printf ( ">>> Docekan proces %d sa statusom %d\n", (int) pid, status );

	printf ( "<<<\n\n" );


	printf ( ">>> Stvaranje procesa sa: "
		 "'stvori_novi_proces ( 2, \"cat\", \"Makefile\" )' >>>\n" );

	pid = stvori_novi_proces ( 0, 2, "cat", "Makefile" );

	printf ( ">>> Stvoren proces %d\n", (int) pid );

	status = cekaj_kraj_procesa ( pid );

	printf ( ">>> Docekan proces %d sa statusom %d\n", (int) pid, status );

	printf ( "<<<\n\n" );


	return 0;
}
