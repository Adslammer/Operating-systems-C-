/*! lab1.c
 *
 * Kopira sadrzaj jednog direktorija u drugi. Pri tome ce se stare datoteke, ako
 * nisu iste novima, preimenovati tako da se na ime doda vrijeme zadnje promjene
 * (u obliku YYYY-MM-DD_HH-MM-SS), a nova ce se onda tamo kopirati pod
 * orginalnim imenom.
 * Pretpostavka je da se u direktoriju nalaze samo datoteke - inace program nece
 * raditi ispravno. Takodjer, preskacu se datoteke koje pocinju tockom (a to su
 * na *UNIX* sustavima i oznake trenutnog direktorija (.), i onog ispod (..).
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>

#define IME	80
#define BLOK	1000

#include "datoteke.h"

int main ( int argc, char *argv[] )
{
	/* zadano */

	char ime1[IME], ime2[IME], ime3[IME], vrijeme[20];
	int n, i;
	char **dat;
	DIR *test;

	if ( argc != 3 )
	{
		fprintf ( stderr, "Program kopira jedan direktorij u drugi\n" );
		fprintf ( stderr, "Uporaba: %s direkt1 direkt2\n", argv[0] );
		return 1;
	}

	dat = dohvati_datoteke ( argv[1], &n );
	if ( dat == NULL )
		return 2;
	

	if ( ( test = opendir ( argv[2] ) ) == NULL )
	{
		if ( mkdir ( argv[2], 0755 ) ) /* 0755 == rwx r-x r-x */
		{
			perror ( "mkdir" );
			fprintf ( stderr, "Ne moze se stvoriti direktorij %s\n",
				  argv[2]);
			return 3;
		}
	}
	else {
		closedir ( test );
	}
	
	printf ( "Kopiram iz %s -> %s:\n", argv[1], argv[2] );
	
	/* Iteriraj po svim datotekama */
	for ( i = 0; i < n; i++ )
	{
		printf ( "%s\n", dat[i] );

		strcpy ( ime1, argv[1] );
		strcat ( ime1, "/" );
		strcat ( ime1, dat[i] );

		strcpy ( ime2, argv[2] );
		strcat ( ime2, "/" );
		strcat ( ime2, dat[i] );
		/* pretpostavlja se da je ukupna duljina manja od IME ! Bolje bi
		 bilo kada bi se to provjerilo i koristilo strncpy i strncat. */

		if ( usporedi ( ime1, ime2 ) )
		{
			if ( !vrijeme_zadnje_promjene ( ime2, vrijeme ) )
			{
				/* datoteka postoji */
				strcpy ( ime3, ime2 );
				strcat ( ime3, "." );
				strcat ( ime3, vrijeme );
				rename ( ime2, ime3 );
			}

			kopiraj_datoteku ( ime1, ime2 );
		}

		/* sleep (5); za drugu vjezbu */
	}

	free ( dat );

	return 0;
}
