/*! kopiraj.c
 *
 * Kopira sadrzaj jednog direktorija u drugi. Pri tome ce se stare datoteke
 * prepisati novima (ako takve postoje)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>

#include <string.h>
#include <stdio.h>

#define IME	80
#define BLOK	1000

int main ( int argc, char *argv[] )
{
	struct dirent *de = NULL;
	DIR *d = NULL;
	struct stat svojstva;
	struct utimbuf vremena;
	char ime1[IME], ime2[IME], blok[BLOK];
	FILE *d1, *d2;
	int procitano;

	if ( argc != 3 )
	{
		fprintf ( stderr, "Program kopira jedan direktorij u drugi\n" );
		fprintf ( stderr, "Uporaba: %s dir1 dir2\n", argv[0] );
		return 1;
	}

	d = opendir ( argv[1] );
	if ( d == NULL )
	{
		perror ( "opendir" );
		fprintf ( stderr, "Ne moze se otvoriti direktorij %s\n",
			  argv[1] );
		return 2;
	}

	/* stvori direktorij s pravima 0755 = rwxr-xr-x */
	if ( mkdir ( argv[2], 0755 ) )
	{
		perror ( "mkdir" );
		fprintf ( stderr, "Ne moze se stvoriti direktorij %s\n",
			  argv[2] );
		return 3;
	}

	printf ( "Kopiram iz %s -> %s:\n", argv[1], argv[2] );

	/* Iteriraj po svim datotekama */
	while ( ( de = readdir (d) ) != NULL )
	{
		/* preskoci datoteke koje pocinju znakom '.' */
		if ( de->d_name[0] == '.' )
			continue;

		printf ( "%s\n", de->d_name );

		strcpy ( ime1, argv[1] );
		strcat ( ime1, "/" );
		strcat ( ime1, de->d_name );

		/* preskoci datoteke za koje ne mogu dobiti podatke */
		if ( stat ( ime1, &svojstva ) == -1 )
			continue;

		strcpy ( ime2, argv[2] );
		strcat ( ime2, "/" );
		strcat ( ime2, de->d_name );

		d1 = fopen ( ime1, "r" );
		d2 = fopen ( ime2, "w" );

		if ( d1 == NULL || d2 == NULL )
		{
			perror ( "open" );
			fprintf ( stderr, "Greska u otvaranju datoteka!\n"
				  "[%s]->[%s])\n", ime1, ime2 );
			fclose ( d1 ); fclose ( d2 );
			continue;
		}

		while ( !feof (d1) )
		{
			procitano = fread ( blok, 1, BLOK, d1 );
			fwrite ( blok, procitano, 1, d2 );
		}

		fclose ( d1 );
		fclose ( d2 );

		/* azuriraj vremena za kopiranu datoteku, tako da odgovaraju
		   vremenima pocetne datoteke */
		vremena.actime = svojstva.st_atime;
		vremena.modtime = svojstva.st_mtime;

		utime ( ime2, &vremena );
	}

	closedir (d);

	return 0;
}
