/*! datoteke.c
 *
 * Sucelje za rad s datotekama
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>

#include "datoteke.h"
#define BLOK	1000
/*!
 * Dohvaca sadrzaj direktorija = popis datoteka
 * \param direktorij	Ime direktorija (relativno u odnosu na program)
 * \param broj		Adresa gdje ce se pohraniti broj datoteka
 * \returns polje s imenima datoteka, tj. kazaljka na "char *imena[*broj]"
 * Mjesto za imena se zauzima s 'malloc', nakon sto se izracunalo koliko
 * prostora treba za sve [ broj*sizeof(char *) + zbroj velicina za sva imena ]
 */
char **dohvati_datoteke ( const char *direktorij, int *broj ){
	DIR *d = NULL;
	struct dirent *de = NULL;
	struct stat statbuf;
	int j=0;
	char **niz;
	
	d = opendir (direktorij);
	
    while ( ( de = readdir ( d ) ) != NULL ) {
		        /* preskacem datoteke koje pocinju znakom '.' */
                if ( de->d_name[0] == '.' )
                        continue;
          	j++;
    }
	*broj=j;
	closedir (d);
	
	niz=(char**)malloc(j*sizeof(char *));	
	
	d = opendir (direktorij);
	j=0;

	while ( ( de = readdir ( d ) ) != NULL ){
		        /* preskacem datoteke koje pocinju znakom '.' */
                if ( de->d_name[0] == '.' )
                        continue;
		niz[j]=malloc ((strlen(de->d_name)*sizeof(char)+1));
		strcpy(niz[j], de->d_name);
        j++;
    }
	return niz;
}

/*!
 * Usporedjuje dvije datoteke (samo atribute velicine i vremena zadnje promjene)
 * \param ime1	Ime prve datoteke
 * \param ime2	Ime druge datoteke
 * \returns 0, ako su datoke jednake, 1 inace
 */
int usporedi ( const char *ime1, const char *ime2 ){
	struct stat statbuf1,statbuf2;
	//struct tm *timeinfo1,timeinfo2;
	
	if(stat ( ime1, &statbuf1 )!=-1 && stat ( ime2, &statbuf2 )!=-1){
       //timeinfo1 = localtime ( &statbuf1.st_mtime );
	   //timeinfo2 = localtime ( &statbuf2.st_mtime );
	   if(statbuf1.st_size==statbuf2.st_size && statbuf1.st_mtime==statbuf2.st_mtime)
		   return 0;
	}
	return 1;
}

/*!
 * Kopira prvu datoteku u drugu, pritom ocuva i vremena zadnje promjene
 * \param ime1	Ime prve datoteke
 * \param ime2	Ime druge datoteke
 * \returns 0, ako je kopiranje uspjesno, 1 inace
 */
int kopiraj_datoteku ( const char *ime1, const char *ime2 ){
	struct dirent *de = NULL;
        DIR *d = NULL;
        struct stat svojstva;
        struct utimbuf vremena;
        char blok[BLOK];
        FILE *d1, *d2;
        int procitano;
		
	stat ( ime1, &svojstva ) ;
			
	d1 = fopen ( ime1, "r" );
	d2 = fopen ( ime2, "w" );

	if ( d1 == NULL || d2 == NULL ){
		perror ( "open" );
		fclose ( d1 ); 
		fclose ( d2 );
	}

	while ( !feof (d1) ){
		procitano = fread ( blok, 1, BLOK, d1 );
		fwrite ( blok, procitano, 1, d2 );
	}

	fclose ( d1 );
	fclose ( d2 );
	// azuriraj vremena za kopiranu datoteku, tako da odgovaraju  vremenima pocetne datoteke 
	vremena.actime = svojstva.st_atime;
	vremena.modtime = svojstva.st_mtime;

	utime ( ime2, &vremena );
		
	return 0;
}

/*!
 * Dohvaca i vraca vrijeme zadnje promjene datoteke u formatu:
 * YYYY-MM-DD_HH-MM-SS, npr. 2010-07-21_17-08-05
 * \param ime		Ime datoteke
 * \param vrijeme	Adresa za smjestaj vremena
 * \returns 0, ako je dohvat uspjesan, 1 inace
 */
int vrijeme_zadnje_promjene ( const char *ime, char *vrijeme ){
	struct stat statbuf;
	struct tm *timeinfo;

	if(stat ( ime, &statbuf )!=-1){
	   timeinfo = localtime ( &statbuf.st_mtime );
	   strftime ( vrijeme, 20, "%Y-%m-%d_%H-%M-%S", timeinfo );
	   return 0;
	}
	else return 1;
}
