/*! lab2.c
 *
 * Obradjuje signale SIGUSR1, SIGUSR2 i SIGTERM
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "signali.h"

/*!
 * Primjer funkcije za obradu signala
 * \param sig	Broj signala koji je primljen
 * \param info	Dodatne informacije o signalu (ako nije NULL)
 * \param context	ignorirati :) (ili pogledati man stranice za sigaction)
 */
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
		for(i=0; i<10; i++){
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
		for(i=0;i<10;i++){
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
		printf("SIGTERM \nProces je gotov: SIGUSR1 = %d \n             SIGUSR2 = %d\n",sig1,sig2);
		kill(getpid(),SIGKILL); // moze i kill(0,SIGKILL)
    }

	odblokiraj_signal(SIGUSR1);
	odblokiraj_signal(SIGUSR2);
}

int main (int argc,char **argv){

	printf ( "Proces %ld poceo\n", (long) getpid() );

	if(SIGUSR1)
		registriraj_signal ( SIGUSR1, obrada, NULL );
	if(SIGUSR2)
		registriraj_signal ( SIGUSR2, obrada, NULL );
	if(SIGTERM)
		registriraj_signal ( SIGTERM, obrada, NULL );

	for (;;)
		sleep (1);

	return 0;
}

