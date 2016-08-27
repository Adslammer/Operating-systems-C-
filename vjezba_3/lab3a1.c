#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


extern int random_prime();

void* funkcija(int* broj){
	int i,j;
	for(i=0;i<(*broj);i++){
		j=(int) pthread_self();
		j%=10;
		j*=(-1);
		printf("dretva tid= %d, broj=%d\n",j,(int) random_prime());
		sleep(1);
	}
	j=(int) pthread_self();
	j%=10;
	j*=(-1);
	printf("dretva tid=%d je zavrsila\n",j);
}

int main(int argc, char **argv){
	int d,s,a;
	pthread_t thr_id[(atoi(argv[1]))];
	d=atoi(argv[1]);
	s=atoi(argv[2]);
	printf("Stvaram dretve\n");
	sleep(5);
	printf("Posao stvoren\n");
	for(a=0;a<d;a++){
		if(pthread_create ( &thr_id[a],NULL,(void*)funkcija, &s )!= 0 ){
			printf("Ne moze stvorit dretvu\n");
			exit(1);
		}
	}
	for(a=0;a<d;a++){
		pthread_join ( thr_id[a], NULL );
	}
	return 0;
}
