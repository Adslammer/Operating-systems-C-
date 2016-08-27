/*! izlistaj.c
 *
 * Ispisuje sadrzaj (datoteke) nekog direktorija
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#include <string.h>
#include <stdio.h>

#define IME	80

int main ( int argc, char *argv[] )
{
	struct dirent *de = NULL;
	DIR *d = NULL;
	struct stat statbuf;
	struct tm *timeinfo;
	char mts[20], ime[IME];

	if ( argc != 2 )
	{
		fprintf ( stderr, "Uporaba: %s direktorij\n", argv[0] );
		return 1;
	}

	d = opendir ( argv[1] ); /* 'otvara' direktorij */
	if ( d == NULL )
	{
		perror ( "opendir" );
		fprintf ( stderr, "Ne moze se otvoriti direktorij %s\n",
			  argv[1] );
		return 2;
	}

	printf ( "Velicina\tZadnja promjena\t\tIme\n"
		 "--------------------------------------------------------\n" );

	/* Iterira po svim datotekama u direktoriju */
	while ( ( de = readdir ( d ) ) != NULL )
	{
		/* preskoci datoteke koje pocinju znakom '.' */
		if ( de->d_name[0] == '.' )
			continue;

		strcpy ( ime, argv[1] );
		strcat ( ime, "/" );
		strcat ( ime, de->d_name );

		/* dohvati podatke o datoteci */
		if ( stat ( ime, &statbuf ) == -1 )
			continue;

		printf ( "%-16lld", (long long) statbuf.st_size );

		/* pretvori vrijeme u znakovni niz */
		timeinfo = localtime ( &statbuf.st_mtime );
		strftime ( mts, 20, "%Y-%m-%d_%H-%M-%S", timeinfo );

		printf ( "%s\t", mts );

		printf ( "%s\n", de->d_name );
	}

	closedir (d);

	return 0;
}
