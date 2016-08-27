#include "logiranje.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>
#define BR_DRETVI	4

#define IME	80
#define BLOK	1000

#include "dretve_i_procesi.h"

#include "datoteke.h"



int main2 (char *argv[], int argc)
{
	/* zadano */

	char ime1[IME], ime2[IME], ime3[IME], vrijeme[20];
	int n, i,pom;
	char **dat;
	void* dretve;

	DIR *test;
	dretve = stvori_radne_dretve ( BR_DRETVI );
	//printf("Stvorio je dretve\n");
	if ( argc != 3 )
	{
		LOGIRAJ_GRESKU("Program kopira jedan direktorij u drugi\n" );
		LOGIRAJ_GRESKU("Uporaba: %s direkt1 direkt2\n", argv[0] );
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
			LOGIRAJ_GRESKU("Ne moze se stvoriti direktorij %s\n",argv[2]);
			return 3;
		}
	}
	else {
		closedir ( test );
	}

	LOGIRAJ_GRESKU( "Kopiram iz %s -> %s:\n", argv[1], argv[2] );
	/* Iteriraj po svim datotekama */
	//printf("ide iterirat\n");
	for ( i = 0; i < n; i++ )
	{
		LOGIRAJ_GRESKU( "%s\n", dat[i] );
		//printf("e sad ako mi se ovo pojavi onda ide sve dobro\n");
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

			dodaj_posao_kopiranja (dretve, ime1, ime2 );
		}

		/* sleep (5); za drugu vjezbu */
	}

	cekaj_dovrsetak_poslova ( dretve, 1 );
	LOGIRAJ_GRESKU("Poslovi gotovi\n");

	makni_radne_dretve ( dretve );


	free ( dat );

	return 0;
}

int main ( int argc, char *argv[] )
{
        stvori_proces_dijete ( main2, argv, argc, 1 );

        return 0;
}
