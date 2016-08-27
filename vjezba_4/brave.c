/*! Zakljucavanje datoteka */



#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define _BRAVE_C_

#include "brave.h"
#include "first_fit.h"

/* lista je slozena po imenima! */

static brave_t *brave = NULL;


static pthread_condattr_t cattr;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


#define VELZS	( ( sizeof (brava_t) + 30 ) * 100 )
#define ZSIME	"/stud_0036450452"


/*!

 * Inicijalizacija zajednickog spremnika i sl.

 * Ne poziva se izravno, vec iz ostalih funkcija (kao prva stvar)

 */

static void inicijaliziraj_brave() {
    /* zadano */
    char *imezs;
    int id;

	pthread_mutexattr_t mattr;

    pthread_mutex_lock(&mutex);

    if (brave) {

		pthread_mutex_unlock(&mutex);

		return; /* vec je inicijalizirano i u ovom procesu */

	}


    imezs = getenv("ZS_IME"); /* dohvati varijablu okoline */

    if (imezs == NULL)
          imezs = ZSIME;


    /* otvori (ili i stvori) opisnik zajednickog spremnika (shm_open),

	 * postavi velicinu (ftruncate); pogledati u '/dev/shm' (na Linuxu) */

	id = shm_open(imezs, O_CREAT | O_RDWR, 00600);

	/* parametri: ime datoteke; 'stvori ako ne postoji', 'za citanje i

	 * pisanje'; ako se datoteka stvara postavi dozvole: rw- --- --- */


    if (id == -1 || ftruncate(id, VELZS) == -1) {
        pthread_mutex_unlock(&mutex);
        return;
    }


    /* 'mapiraj' zadanu datoteku (id) u adresni prostor procesa */

	brave = (brave_t *) mmap(NULL, VELZS, PROT_READ | PROT_WRITE, MAP_SHARED,

			id, 0);

	/* parametri: NULL => 'postavi bilo gdje'; velicina mapiranog dijela,

	 * 'za citanje i pisanje'; 'djeljeno medju procesima'; id => datoteka;

	 * pomak od pocetka datoteke (0 - od pocetka) */



	if (brave == (void *) -1) {
        pthread_mutex_unlock(&mutex);
        return;
    }

	close(id);


    /* ako nije inicijalizirano, inicijaliziraj */

	if (brave->inicijalizirano != ZS_INIC) {
        brave->prva = brave->zadnja = NULL;


        /* Potrebno je definirati da se istim objektom sinkroniziraju

		 * dretve i razlicitih procesa! Sam objekt je u zajednickom

		 * spremniku! */

		pthread_mutexattr_init(&mattr);

		pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

		pthread_mutex_init(&brave->gl_brava, &mattr);


        ff_init(brave + 1, VELZS - sizeof(brave_t), 1);


        brave->inicijalizirano = ZS_INIC;

	} else {
        /* samo azuriraj lokalne variable (kazaljku) za ovaj proces */

		ff_init(brave + 1, VELZS - sizeof(brave_t), 1);

	}



	pthread_condattr_init(&cattr);

	pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_unlock(&mutex);

}



/*!

 * Zakljucaj zadano ime

 * \param ime Ime koje se zakljucava (i 'logicki' povezuje s imenom datoteke)

 * \param kako Nacin zakljucavanja (SAMOCITAJ, NEMJIME, MJSADRZAJ, EKSKLUZIV)

 * \param cekaj Blokirati ili ne dok se brava ne zauzme ?

 * \returns Pokazivac na objekt zakljucavanja ili NULL ako nije zakljucano

 */

void *zakljucaj(const char *ime, int kako, int cekaj) {

	brava_t *iter, *nova;
    int usporedba, konflikt, i;

    inicijaliziraj_brave();

	ZAKLJUCAJ_BRAVE();

    iter = brave->prva;

	while (iter != NULL) {

		/* ima li 'iter' veze s ovim pokusajem zakljucavanja? */

#define NEDEFINIRANO	0

#define RAZLICITO	1

#define NAD_DIR		2

#define POD_DIR		3

#define ISTO		4


        usporedba = NEDEFINIRANO;


        /* usporedi imena */

		for (i = 0; usporedba == NEDEFINIRANO && i < NAJDUZEIME; i++) {

			/* jesmo dosli do kraja jednog od nizova? */

			if (ime[i] == 0 || BRAVA(iter)->ime[i] == 0) {

				if (BRAVA(iter)->ime[i] == '/')

					usporedba = NAD_DIR;

				else if (ime[i] == '/')

					usporedba = POD_DIR;

				else if ((ime[i] | BRAVA(iter)->ime[i]) == 0)

					usporedba = ISTO;

				else

					usporedba = RAZLICITO;

			} else if (ime[i] != BRAVA(iter)->ime[i]) {

				usporedba = RAZLICITO;

			}

			/* else usporedba = NEDEFINIRANO; */

		}

        switch (usporedba) {

			case ISTO: /* zakljucavanje nad istim objektom */

				konflikt = KONFLIKT_ISTI(BRAVA(iter)->kako, kako);

				break;


            case POD_DIR: /* zakljucavanje datoteke ili direktorija

			 koji se nalaze u zakljucanom direktoriju */

				konflikt = KONFLIKT_DIR(BRAVA(iter)->kako, kako);

				break;


            case NAD_DIR: /* zakljucavanje direktorija u kojem se

			 nalazi u zakljucani objekt */

				konflikt = KONFLIKT_DIR(kako, BRAVA(iter)->kako);

				break;


            default: /* RAZLICITO i NEDEFINIRANO */

				konflikt = 0;

				break;

		}


        if (konflikt) {

			/* ZA NAPRAVITI:

			 ako ne treba blokirati (cekaj je nula)

			 vrati NULL;

			 inace

			 blokiraj dretvu nad ovom bravom (iter);

			 ponovno provjeri SVE brave! */


            if (cekaj == 0) 
                return NULL;
			
			else {
                CEKAJ_BRAVU( BRAVA( iter ) );
				iter = brave->prva;
			}

		} else
        	iter = BRAVA(iter)->iduca;

	}



	/* nema sukoba, zakljucaj, tj. napravi bravu i stavi je na kraj reda */

	nova = ff_alloc(sizeof(brava_t) + strlen(ime) + 1);

	nova->kako = kako;

	nova->pid = getpid();

	pthread_cond_init(&nova->red, &cattr);

	nova->ref = 1;

	strcpy(&nova->ime[0], ime);


    /* dodaj na kraj liste */

	nova->preth = brave->zadnja;

	nova->iduca = NULL;


    if (brave->zadnja != NULL)
        BRAVA(brave->zadnja)->iduca = POMAK(nova);

	else
        brave->prva = POMAK(nova); /* lista je bila prazna */


    brave->zadnja = POMAK(nova);


    OTKLJUCAJ_BRAVE();


    return nova;

}



/*!

 * Otkljucava zadani objekt

 * \param brava Pokazivac na objekt zakljucavanja

 */

void otkljucaj(void *brava) {

	/* zadano */

	brava_t *b;



	if (brava == NULL) {

		return;

	}



	inicijaliziraj_brave();



	b = brava;



	ZAKLJUCAJ_BRAVE();



	/* makni 'b' iz liste */

	if (b->preth != NULL)

		BRAVA(b->preth)->iduca = b->iduca;



	if (b->iduca != NULL)

		BRAVA(b->iduca)->preth = b->preth;



	if (brave->prva == POMAK(b))

		brave->prva = b->iduca;



	if (brave->zadnja == POMAK(b))

		brave->zadnja = b->preth;



	/* oslobodi sve koji cekaju na ovoj bravi (ako ih ima), i oslobodi

	 * prostor koji ona koristi (ako nema blokiranih dretvi) */

	OBRISI_BRAVU(b);



	OTKLJUCAJ_BRAVE();

}



/*!

 * Ispisuje postojece kljuceve

 */

void ispisi_brave() {

	brava_t *iter;

	int terumi;



	inicijaliziraj_brave();



	if (brave == NULL) {

		printf("Nema brava!\n");

		return;

	}



	/* ZA NAPRAVITI - ako lista nije prazna, iterirati po listi i ispisati

	 sve brave, tj. za svaki napisati nacin, PID procesa koji ju je

	 postavio te ime objekta kojeg stiti */



	if (brave->prva != NULL) {

		terumi = 0;

		iter = BRAVA(brave->prva);



		ZAKLJUCAJ_BRAVE();



		



		do {



			if( terumi==1 ) {

				iter = BRAVA(iter->iduca);

			}



			
				if(iter->kako==SAMOCITAJ){

					printf(

							" Nacin: SAMOCITAJ;\n PID procesa: %d;\n Stiti objekt: %s\n\n",

							(int) iter->pid, iter->ime);

				}



				else if(iter->kako==NEMJIME){

					printf(

							" Nacin: NEMJIME;\n PID procesa: %d;\n Stiti objekt: %s\n\n",

							(int) iter->pid, iter->ime);

			}



				else if(iter->kako==MJSADRZAJ){

					printf(

							" Nacin: MJSADRZAJ;\n PID procesa: %d;\n Stiti objekt: %s\n\n",

							(int) iter->pid, iter->ime);

					}



				else if(iter->kako==EKSKLUZIV){
					printf(

							" Nacin: EKSKLUZIV;\n PID procesa: %d;\n Stiti objekt: %s\n\n",

							(int) iter->pid, iter->ime);

				}




			



			terumi = 1;

		} while (iter->iduca != NULL);



		OTKLJUCAJ_BRAVE();

	}

	else {

		printf( "Trenutno nema aktivnih brava!!\n\n" );

	}
}



/*!

 * Brise sve brave

 * (prikazani kod samo obrise zajednicki spremnik, bilo bi bolje proci kroz sve

 *  i odblokirati dretve koje eventualno cekaju)

 */

void brisi_sve_brave() {

	/* zadano */

	char *imezs;



	inicijaliziraj_brave();



	/* samo obrisi zajednicki spremnik i sve ce nestati (FIXME) */

	munmap((void *) brave, VELZS);



	imezs = getenv("ZS_IME");

	if (imezs == NULL)

		imezs = ZSIME;



	shm_unlink(imezs);



	brave = NULL;

}
