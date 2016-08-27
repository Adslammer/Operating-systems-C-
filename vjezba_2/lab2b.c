/*! lab2b.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "signali.h"
//koristim funkciju pošalji kojom šaljem signal programu lab2a.c
//moram omoguèiti promjenu parametra i taj parametar slati u funkciju pošalji

int main (int argc, char **argv){
	int param=0;
	int odabrani_broj;
	int pid=atoi(argv[1]);

	while(1){
		printf("Odaberi signal: 1-SIGUSR1, 2-SIGUSR2, 3-SIGTERM \n(ili s 0 promjeni parametar, param=%d)\n",param);
		scanf("%d",&odabrani_broj);

		if(odabrani_broj==0){
			printf("Unesi novu vrijednost parametra:");
			scanf("%d",&param);
		}
		else if(odabrani_broj==1)
			posalji_signal(SIGUSR1,pid,param);
		else if(odabrani_broj==2)
			posalji_signal(SIGUSR2,pid,param);
		else if(odabrani_broj==3)
			posalji_signal(SIGTERM,pid,param);
	}

	return 0;
}

