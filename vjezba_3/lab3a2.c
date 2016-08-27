#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


extern int random_prime();

int main(int argc, char **argv){
	int d,s,a,b;
	d=atoi(argv[1]);
	s=atoi(argv[2]);
	for(a=0;a<d;a++){
		switch(fork()){

			case 0:
				for(b=0;b<=s;b++){
					if(b==s) printf("Zavrsio proces pid=%d",getpid());
					else printf("proces pid=%d broj=%d ",getpid(),random_prime());
					sleep(1);
					printf("\n");
				}
				exit(0);
			case -1:
				printf("ne moze stvoriti proces\n");
		}
	}
	while(a--){
		wait(NULL);
	}

	return 0;
}
