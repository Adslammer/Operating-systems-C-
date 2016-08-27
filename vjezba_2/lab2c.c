

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>



#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#include "signali.h"


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

void obrada ( int sig, siginfo_t *info, void *context ){
	static int sig1;
	static int sig2;
	int i;
	
	printf ( "Primio signal %d\n", sig );

	if ( info != NULL && info->si_code == SI_QUEUE )
		printf ( " uz parametar = %d (%p)\n", info->si_value.sival_int,info->si_value.sival_ptr );
	
	blokiraj_signal(SIGUSR1);
	blokiraj_signal(SIGUSR2);
	
	if(sig==SIGUSR1){	
		for(i=0;i<=9;i++){
			if(i==0){
				printf("SIGUSR1 - obrada signala uz parametar = %d (%p)\n", info->si_value.sival_int,info->si_value.sival_ptr );
				sleep(1);
			}
			else if(i==9){
				printf("SIGUSR1 - obrada signala zavrsena\n");
				sig1+=1;
				sleep(1);				
			}
			else{
				printf("SIGUSR1 - obrada signala %d/%d\n",i,8);
				sleep(1);
			}
		}
		printf("\n\n");
	}
	else if(sig==SIGUSR2){
		for(i=0;i<=9;i++){
			if(i==0){
				printf("SIGUSR2 - obrada signala uz parametar = %d (%p)\n", info->si_value.sival_int,info->si_value.sival_ptr );
				sleep(1);
			}
			else if(i==9){
				printf("SIGUSR2 - obrada zavrsena\n");
				sig2+=1;
				sleep(1);
			}
			else{
				printf("SIGUSR2 - obrada signala %d/%d\n",i,8);
				sleep(1);
			}
		}
		printf("\n\n");
	}
	else if(sig==SIGTERM){
		printf("SIGTERM \nProces je gotov: SIGUSR1 = %d \n                 SIGUSR2 = %d\n",sig1,sig2);
		kill(getpid(),SIGKILL); // ili kill(0,SIGKILL)
    }
	
	odblokiraj_signal(SIGUSR1);
	odblokiraj_signal(SIGUSR2);					
}


#define IME	80
#define BLOK	1000

#include "datoteke.h"

int main ( int argc, char *argv[] ){
	
	printf ( "Proces %ld poceo\n", (long) getpid() );
	if(SIGUSR1)
		registriraj_signal ( SIGUSR1, obrada, NULL );
	if(SIGUSR2)
		registriraj_signal ( SIGUSR2, obrada, NULL );
	if(SIGTERM)
		registriraj_signal ( SIGTERM, obrada, NULL );

	/* zadano */

	char ime1[IME], ime2[IME], ime3[IME], vrijeme[20];
	int n, i;
	char **dat;
	DIR *test;

	if ( argc != 3 ){
		fprintf ( stderr, "Program kopira jedan direktorij u drugi\n" );
		fprintf ( stderr, "Uporaba: %s direkt1 direkt2\n", argv[0] );
		return 1;
	}

	dat = dohvati_datoteke ( argv[1], &n );
	if ( dat == NULL )
		return 2;

	if ( ( test = opendir ( argv[2] ) ) == NULL ){
		if ( mkdir ( argv[2], 0755 ) ){
			perror ( "mkdir" );
			fprintf ( stderr, "Ne moze se stvoriti direktorij %s\n",
				  argv[2]);
			return 3;
		}
	}
	else closedir ( test );

	printf ( "Kopiram iz %s -> %s:\n", argv[1], argv[2] );

	/* Iteriraj po svim datotekama */
	for ( i = 0; i < n; i++ ){
		printf ( "%s\n", dat[i] );

		strcpy ( ime1, argv[1] );
		strcat ( ime1, "/" );
		strcat ( ime1, dat[i] );

		strcpy ( ime2, argv[2] );
		strcat ( ime2, "/" );
		strcat ( ime2, dat[i] );
		/* pretpostavlja se da je ukupna duljina manja od IME ! Bolje bi
		 bilo kada bi se to provjerilo i koristilo strncpy i strncat. */

		if ( usporedi ( ime1, ime2 ) ){
			if ( !vrijeme_zadnje_promjene ( ime2, vrijeme ) ){
				/* datoteka postoji */
				strcpy ( ime3, ime2 );
				strcat ( ime3, "." );
				strcat ( ime3, vrijeme );
				rename ( ime2, ime3 );
			}
            kopiraj_datoteku ( ime1, ime2 );
		}

		 sleep (5);
	}

	free ( dat );
    return 0;
}
