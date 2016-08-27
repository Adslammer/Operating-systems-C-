/*!
 * Dretve i procesi
 */

#include "dretve_i_procesi.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>



/*!
 * Sucelje za rad s procesima
 */

static int postavi_kao_servis ();


/*!
 * Stvaranje novog procesa dupliciranjem postojeceg
 * \param funkc Funkcija koju ce novi proces izvoditi
 * \param pparam Parametar za funkciju - pokazivac
 * \param iparam Parametar za funkciju - broj
 * \returns PID stvorenog procesa
 */
pid_t stvori_proces_dijete ( void *funkcija, void *pparam, int iparam,
			     int servis )
{
	pid_t pid;
	void (*f) (void *, int);

	pid = fork ();

	if ( pid )
		return pid;

	/* treba li biti servis? */
	if ( servis && postavi_kao_servis () )
	{
		perror ( "postavi_kao_servis" );
		exit ( EXIT_FAILURE );
	}

	f = funkcija;

	f ( pparam, iparam );

	exit ( EXIT_SUCCESS );
}

/*!
 * Stvaranje novog procesa pokretanjem programa
 * \param br_arg Broj argumenata (svi su znakovni nizovi!)
 * \param arg Prvi argument (ime datoteke s programom)
 * \returns PID stvorenog procesa
 */
pid_t stvori_novi_proces ( int servis, int br_arg, char *arg, ... )
{
	va_list ap;
	char *args[br_arg + 1];
	pid_t pid;
	int i;

	pid = fork ();

	if ( pid )
		return pid;

	args[0] = arg;
	va_start ( ap, arg );
	for ( i = 1; i < br_arg; i++)
		args[i] = va_arg ( ap, char *); /* moglo je i arg[i] */
	va_end ( ap );

	args[br_arg] = NULL; /* mora zavrsavati s NULL */

	/* treba li biti servis? */
	if ( servis && postavi_kao_servis () )
	{
		perror ( "postavi_kao_servis" );
		exit ( EXIT_FAILURE );
	}

	execvp ( args[0], args );

	perror ( "execv" );   /* nema povratka iz execv osim kada ne uspije */
	exit ( EXIT_FAILURE );
}

/*!
 * Ceka na kraj procesa sa zadanim PID-om
 * \param pid Pid procesa (ili 0, kada ceka bilo koje dijete)
 * \returns status docekanog procesa, -1 u slucaju greske
 */
int cekaj_kraj_procesa ( int pid )
{
	int status;

	if ( waitpid ( pid, &status, 0 ) < 0 )
		return -1;

	return status;
}

/*!
 * Pomocna funkcija koja proces odvaja od konzole - postavlja kao servis
 */
static int postavi_kao_servis ()
{
	freopen("/dev/null","r",stdin);
	freopen("/dev/null","w",stdout);
	freopen("/dev/null","w",stderr);
	setsid();
	return 0;
}


/*!
 * Sucelja za rad s dretvama
 */

static void *radna_dretva ( void *pskup );

/* lista opisnika skupova radnih dretvi */
static radne_dretve_t *rd_prvi = NULL, *rd_zadnji = NULL;

/* statička inicijalizacija varijable zaključavanja (mutexa) */
static pthread_mutex_t rd_brava = PTHREAD_MUTEX_INITIALIZER;

/*!
 * Stvara skup radnih dretvi
 * \param broj Broj dretvi u skupu (koliko da ih stvori)
 * \returns Pokazivac na opisnik skupa dretvi
 */
void *stvori_radne_dretve ( int broj )
{
	radne_dretve_t *dretve;
	int i;

	dretve = malloc ( sizeof (radne_dretve_t) + sizeof (pthread_t) * broj );
	dretve->iddr = (pthread_t *) ( dretve + 1 );

	dretve->broj = broj;

	pthread_mutex_init ( &dretve->brava, NULL );
	pthread_cond_init ( &dretve->red, NULL );
	pthread_cond_init ( &dretve->gotovo, NULL );

	dretve->prvi = dretve->zadnji = NULL; /*u početku je red poslova prazan*/
	dretve->kraj = 0;
	dretve->br_poslova = 0;

	for ( i = 0; i < broj; i++ ) /* stvori radne dretve */
	{
		if ( pthread_create ( &dretve->iddr[i], NULL, radna_dretva,
			dretve ) )
		{
			printf ( "Greska u stvaranju dretvi!\n" );
			exit ( 1 );
		}
	}

	/* dodaj skup dretvi u listu opisnika skupova dretvi */
	pthread_mutex_lock ( &rd_brava );

	if ( rd_zadnji == NULL )
		rd_prvi = dretve;
	else
		rd_zadnji->iduci = dretve;

	rd_zadnji = dretve;
	dretve->iduci = NULL;

	pthread_mutex_unlock ( &rd_brava );

	return dretve;
}

/*!
 * Radna dretva (kucanski poslovi cekanja i preuzimanja posla)
 * \param odretve Opisnik skupa dretvi (kome dretva pripada)
 * \returns NULL
 */
static void *radna_dretva ( void *odretve )
{
	radne_dretve_t *dretve;
	posao_t *posao;

	dretve = odretve;

	pthread_mutex_lock ( &dretve->brava );

	while ( dretve->kraj == 0 )
	{
		if ( dretve->prvi != NULL ) /* ima li poslova u redu? */
		{
			posao = dretve->prvi; /* uzmi prvi posao */
			/* ažuriraj listu */
			dretve->prvi = dretve->prvi->iduci;
			if ( dretve->prvi == NULL )
				dretve->zadnji = NULL;

			/* prije obrade izadji iz monitora */
			pthread_mutex_unlock ( &dretve->brava );

			/* pocni obradu */
			posao->funkcija ( posao->id, posao->pparam,
					  posao->iparam );

			/* obrada je gotova */
			pthread_mutex_lock ( &dretve->brava );

			dretve->br_poslova--;

			free ( posao );

			/* ako je ovo bio zadnji posao, pusti sve dretve koje su
			   čekale na kraj slova */
			if ( dretve->br_poslova == 0 )
				pthread_cond_broadcast ( &dretve->gotovo );
		}
		else { /* nema poslova pa čekaj */
			pthread_cond_wait ( &dretve->red, &dretve->brava );
		}
	}

	pthread_mutex_unlock ( &dretve->brava );

	return NULL; /* završetak radne dretve */
}

/*!
 * Dodaje posao skupu dretvi
 * \param dretve Opisnik skupa dretvi

 * \param funkcija Kazaljka na funkciju koja obavlja posao
	opcionalni parametri posla:
 * \param pparam Prvi parametar funkcije (kazaljka, može biti NULL)
 * \param iparam Drugi parametar funkcije (cijeli broj)
 */
void dodaj_posao ( void *dretve, void *funkcija, void *pparam, int iparam )
{
	posao_t *posao;
	radne_dretve_t *skup;
	static unsigned int zadnji_broj = 1; /* za dodjelu ID poslu */

	posao = malloc ( sizeof (posao_t) );

	posao->id = zadnji_broj++;
	posao->funkcija = funkcija;
	posao->pparam = pparam;
	posao->iparam = iparam;

	skup = dretve;

	/* dodaj posao u red poslova unutar skupa dretvi */
	pthread_mutex_lock ( &skup->brava );

	if ( skup->zadnji == NULL )
		skup->prvi = posao;
	else
		skup->zadnji->iduci = posao;

	skup->zadnji = posao;
	posao->iduci = NULL;
	skup->br_poslova++;

	pthread_mutex_unlock ( &skup->brava );

	/* signaliziraj radnim dretvama da se pojavio novi posao */
	pthread_cond_signal ( &skup->red );
}

/*!
 * Blokira pozivajucu (glavnu) dretvu dok svi poslovi iz skupa dretvi ne budu
 * gotovi
 * \param dretve Opisnik skupa dretvi
 * \param cekaj Oznaka da li blokirati (cekaj != 0) ili ne (cekaj == 0)
 *		pozivajucu dretvu, ako poslovi jos nisu gotovi
 * \returns status, 0 ako su poslovi dovrseni, 1 inace (kada je cekaj == 0)
 */
int cekaj_dovrsetak_poslova ( void *dretve, int cekaj )
{
	radne_dretve_t *glava;

    glava=dretve; // ne smijem mjenjat
    if (cekaj==0){
		if(glava->prvi !=NULL){
			return 1;
		}
	}
	pthread_mutex_lock (&glava->brava);
	if (glava->prvi !=NULL){
		pthread_cond_wait(&glava->gotovo, &glava->brava);
	}
	pthread_mutex_unlock(&glava->brava);
	return 0;
}


/*!
 * Brise skup radnih dretvi (postavlja oznaku 'kraj' i ceka njihov zavrsetak
 * (pthread_join) te briše opisnik skupa radnih dretvi iz liste opisnika)
 * \param dretve Opisnik skupa dretvi
 * \returns 0 (status)
 */
int makni_radne_dretve ( void *dretve )
{
	int i;
	radne_dretve_t *glava;
	glava=dretve;
	pthread_mutex_lock(&glava->brava);
	glava->kraj=1;

	pthread_cond_broadcast(&glava->red);

	pthread_mutex_unlock(&glava->brava);

	for(i=0;i<(glava->broj);i++){
		pthread_join(glava->iddr[i],NULL);
	}

	//pthread_mutex_destroy(glava->brava);
	pthread_mutex_lock ( &rd_brava );

	while ( rd_prvi != NULL){
        glava = rd_prvi->iduci;
        free ( rd_prvi );
        rd_prvi = glava;
    }

	free(glava);

	pthread_mutex_unlock ( &rd_brava );

	return 0;
}





