/*!
 * Dretve i procesi
 */

//#pragma once

/*!
 * Sucelje za rad s procesima
 */

#include <sys/types.h>

pid_t stvori_proces_dijete ( void *funkcija, void *pparam, int iparam,
			     int servis );
pid_t stvori_novi_proces ( int servis, int br_arg, char *arg, ... );
int cekaj_kraj_procesa ( int pid );

/*!
 * Sucelje za rad s dretvama
 */

#include <pthread.h>

void *stvori_radne_dretve ( int broj );
int makni_radne_dretve ( void *dretve );
void dodaj_posao ( void *dretve, void *funkcija, void *pparam, int iparam );
int cekaj_dovrsetak_poslova ( void *dretve, int cekaj );


/*! Opisnik jednog posla */
typedef struct _posao_
{
	unsigned int id; 	/* id posla */

	/* posao je zadan ovom funkcijom i iducim parametrima
		funkcija posla prima tri parametra:
		prvi je redni broj posla
		drugi i treći su proizvoljni parametri koji se funkciji mogu
		poslati za prijenos podataka
	*/
	void (*funkcija) (int, void *, int);
	void *pparam;
	int iparam;

	struct _posao_ *iduci;	/* za ostvarenje liste */
}
posao_t;

/*! Opisnik skupa radnih dretvi */
typedef struct _radne_dretve_
{
	int broj;		/* broj dretvi u skupu */
	pthread_t *iddr;	/* opisnici dretvi */
	pthread_mutex_t brava;	/* za zakljucavanja pri radu s dretvama */
	pthread_cond_t red;	/* kada nema posla dretve ovdje cekaju */
	pthread_cond_t gotovo;  /* u ovom redu čeka glavna dretva koja je dodala
				   poslove i čeka na završetak svih zadanih
				   poslova */
	int kraj;	/* oznaka kraja rada koju radne dretveprovjeravaju
			 (kada je kraj = 1, radne dretve završavaju s radom)  */

	posao_t *prvi, *zadnji;	/* lista poslova */
	int br_poslova;		/* broj nedovršenih poslova */

	struct _radne_dretve_ *iduci;	/* za ostvarenje listi skupina radnih
					   dretvi */
}
radne_dretve_t;



