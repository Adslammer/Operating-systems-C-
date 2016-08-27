/*! datoteke.c
 *
 * Sucelje za rad s datotekama
 */
#include "datoteke.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>

#define BLOK 1000

/*!
 * Dohvaca sadrzaj direktorija = popis datoteka
 * \param direktorij	Ime direktorija (relativno u odnosu na program)
 * \param broj		Adresa gdje ce se pohraniti broj datoteka
 * \returns polje s imenima datoteka, tj. kazaljka na "char *imena[*broj]"
 * Mjesto za imena se zauzima s 'malloc', nakon sto se izracunalo koliko
 * prostora treba za sve [ broj*sizeof(char *) + zbroj velicina za sva imena ]
 */

typedef struct de {
            
            char *part1;
            char *part2;
            
    } podt;

void *pozovi_kopiraj_datoteku ( int id, void *pparam, int iparam )
{
     
        podt *cell;
        cell = pparam;
        
        printf("Obavljam posao %d\n", id );
        kopiraj_datoteku ( cell -> part1, cell -> part2 );
        free ( cell -> part1 );
        free ( cell -> part2 );
	return NULL;
        
}     

        
void dodaj_posao_kopiranja ( void *dretve, char *ime1, char *ime2 ) 
{
        podt *podat;
        
        podat = ( podt* )malloc ( sizeof ( podt ) );

	podat->part1=(char *)malloc (sizeof(char)*strlen(ime1));
	podat->part2=(char *)malloc (sizeof(char)*strlen(ime2));

        
        strcpy ( podat -> part1, ime1);
        strcpy ( podat -> part2, ime2);
        
        dodaj_posao(dretve, pozovi_kopiraj_datoteku, podat, 0);
        
}


char **dohvati_datoteke ( const char *direktorij, int *broj )
{
	DIR* di;
	int i=0;
	void** podaci;
	int char_num=0; 
	int pomak=0;
	struct dirent* dat;
	*broj=0;
	di= opendir(direktorij);
	if(di==NULL)
	{
		printf("Stream NULL");
		return NULL;
	}
	while((dat=readdir(di))!=NULL){
		if(dat->d_name[0]=='.'){
			continue;
		}
		(*broj)++;
		char_num += strlen(dat->d_name)+1;
	}
	podaci = malloc(sizeof(char*)*(*broj)+ char_num);
	rewinddir(di);
	while((dat= readdir(di) )!=NULL)
	{
		if(dat->d_name[0]=='.')
		{
			continue;
		}
		podaci[i++] = strcpy( (char*) (&podaci[*broj] + pomak ), dat->d_name);
		pomak += strlen(dat->d_name) + 1;
	}
	closedir(di);
	return (char**)podaci;
}

/*!
 * Usporedjuje dvije datoteke (samo atribute velicine i vremena zadnje promjene)
 * \param ime1	Ime prve datoteke
 * \param ime2	Ime druge datoteke
 * \returns 0, ako su datoke jednake, 1 inace
 */
int usporedi ( const char *ime1, const char *ime2 )
{
	struct stat buff_one;
	struct stat buff_two;
	stat(ime1,&buff_one);
	stat(ime2,&buff_two);
	if(buff_one.st_size==buff_two.st_size)
	{
		if(buff_one.st_mtime==buff_two.st_mtime)
		{
			return 0;
		}
	}
	return 1;
}
	

/*!
 * Kopira prvu datoteku u drugu, pritom ocuva i vremena zadnje promjene
 * \param ime1	Ime prve datoteke
 * \param ime2	Ime druge datoteke
 * \returns 0, ako je kopiranje uspjesno, 1 inace
 */
int kopiraj_datoteku ( const char *ime1, const char *ime2 )
{
	struct utimbuf time;
	FILE* dat_one;
	FILE* dat_two;
	struct stat buffer;
	int procitano;
	char blok[BLOK];
	if((dat_one=fopen(ime1,"r"))==NULL)
	{
		printf("Ne može otvorit datoteku ime1 %s",ime1);
		return 1;
	}

	if((dat_two=fopen(ime2,"w"))==NULL)
	{
		printf("Ne moze otvorit datoteku ime2 %s",ime2);
		return 1;

	}
	while ( !feof (dat_one) )
                {
                        procitano = fread ( blok, 1, BLOK, dat_one );
                        fwrite ( blok, procitano, 1, dat_two );
                }



	stat(ime1,&buffer);

	fclose(dat_one);
	fclose(dat_two);

	time.actime=buffer.st_atime;
	time.modtime=buffer.st_mtime;

	utime(ime2,&time);

	return 0;



}

/*!
 * Dohvaca i vraca vrijeme zadnje promjene datoteke u formatu:
 * YYYY-MM-DD_HH-MM-SS, npr. 2010-07-21_17-08-05
 * \param ime		Ime datoteke
 * \param vrijeme	Adresa za smjestaj vremena
 * \returns 0, ako je dohvat uspjesan, 1 inace
 */
int vrijeme_zadnje_promjene ( const char *ime, char *vrijeme )
{
	struct stat buff;
	FILE* dat;
	struct tm *timeinfo;
	if((dat=fopen(ime,"r"))== NULL){
		return 1;
	}
	stat(ime,&buff);
	timeinfo = localtime ( &buff.st_mtime );
	strftime ( vrijeme, 20, "%Y-%m-%d_%H-%M-%S", timeinfo );
	fclose(dat);
	return 0;
}
