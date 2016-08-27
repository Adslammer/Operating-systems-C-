/*! Zakljucavanje datoteka */

void *zakljucaj ( const char *ime, int kako, int cekaj );
void otkljucaj ( void *brava );
void ispisi_brave ();
void brisi_sve_brave ();

/* oznake nacina zaljucavanja */
#define	SAMOCITAJ	1 /* 001 z. za citanje */
#define	NEMJIME		2 /* 010 z. protiv promjene imena  */
#define	MJSADRZAJ	5 /* 101 z. za promjenu sadrzaja (ime ostaje isto) */
#define	EKSKLUZIV	7 /* 111 ekskluzivno z. za sve operacije */

#define OZNAKA		"*CI**M*E"
#define DOHVOZ(i)	({ char *oz = OZNAKA; oz[i]; })
/*
 * Nad istim objektom moze biti vise zakljucavanja (vise brava u listi) ako:
 * a) su to zakljucavanja za citanje i protiv promjene imena
 *	vise SAMOCITAJ i/ili vise NEMJIME moze biti istovremeno
 * b) su to zakljucavanja protiv promjene imena i promjenu sadrzaja
 *	vise NEMJIME i jedan MJSADRZAJ moze biti istovremeno
 *
 *	dozvoljeno	brava1		brava2
 *			SAMOCITAJ	SAMOCITAJ
 *			SAMOCITAJ	NEMJIME
 *			NEMJIME		SAMOCITAJ
 *			NEMJIME		NEMJIME
 *			NEMJIME		MJSADRZAJ
 *			MJSADRZAJ	NEMJIME
 *
 * Nad razlicitm objektima, ali povezanim:direktorij (DIR) i objekt (OBJ) u
 * njemu ili njegovu stablu (nekom poddir.)
 *	dozvoljeno:	brava1-DIR	brava2-OBJ
 *			SAMOCITAJ	SAMOCITAJ
 *			SAMOCITAJ	NEMJIME
 *			SAMOCITAJ	MJSADRZAJ

 *			NEMJIME		(sve)
 */

#ifdef _BRAVE_C_

/* ovako definirane oznake jednoznacno odredjuju konflikte operacijom AND (&) */
#define KONFLIKT_ISTI(STARI, NOVI)	\
(	( STARI != NOVI && ( (STARI) & (NOVI) ) ) || \
	( STARI == NOVI && ( NOVI == MJSADRZAJ || NOVI == EKSKLUZIV ) ) )

#define KONFLIKT_DIR(NAD, POD)		\
	!( NAD == NEMJIME || ( NAD == SAMOCITAJ && POD != EKSKLUZIV ) )

/*! struktura podataka za jedno zakljucavanje - jednu 'bravu' */
typedef struct _brava_
{
	char kako;	/* SAMOCITAJ, NEMJIME, MJSADRZAJ ili EKSKLUZIV */

	pid_t pid;		/* PID procesa koji je to zakljucao */
	pthread_cond_t red;	/* red u kojem dretve cekaju na otkljucavanje */
	unsigned int ref;	/* je li aktivna i koliko jos dretvi koristi ili
				   ceka ovu bravu */
	void *preth, *iduca; /* za ostvarenje liste zakljucavanja */

	char ime[1];	/* tu će biti ime datoteke/direktorija na koji se
			   ovo odnosi, ime moze biti relativno ili ne */
}
brava_t;

/*! lista bravi */
typedef struct _brave_
{
	void *prva;		/* pokazivac na prvu bravu */
	void *zadnja;		/* pokazivac na zadnju bravu */
	pthread_mutex_t gl_brava;	/* pomocna brava */
	unsigned int inicijalizirano; 	/* je li zajednicki spremnik podesen? */
}
brave_t;

/*!
 * Adrese u gornjim strukturama su relativne u odnosu na pocetak, tj. na
 * varijablu 'brave'. Obzirom da je navedeno u zajednickom spremniku koji
 * koristi vise procesa, koji to mogu mapirati na razlicite spremnicke lokacije,
 * to mora biti tako. Zato se koriste iduci makroi.
 */

typedef unsigned long int aint; /* broj duljine jednake adresi */
#define AZBR(a,b)       ( (void *) ( ( (aint) (a) ) + ( (aint) (b) ) ) )
#define AODU(a,b)       ( (void *) ( ( (aint) (a) ) - ( (aint) (b) ) ) )

#define BRAVA(adr)	( (brava_t *) AZBR(adr, brave) )
#define POMAK(adr)	AODU(adr, brave)


#define ZS_INIC	0x55555555	/* 'sve pet' :) */

#define NAJDUZEIME	512

/* sinkronizacija */
#define ZAKLJUCAJ_BRAVE()	pthread_mutex_lock ( &brave->gl_brava )
#define OTKLJUCAJ_BRAVE()	pthread_mutex_unlock ( &brave->gl_brava )
#define CEKAJ_BRAVU(b)							\
{									\
	brava_t *brava = b;						\
	brava->ref++;							\
	pthread_cond_wait (&brava->red, &brave->gl_brava);		\
	brava->ref--;							\
									\
	if ( brava->ref == 0 )						\
		ff_free ( brava );					\
}
#define OBRISI_BRAVU(brava)						\
{									\
	brava->ref--;							\
	if ( brava->ref == 0 )						\
		ff_free ( brava );					\
	else								\
		pthread_cond_broadcast ( &brava->red );			\
}

#endif	/* _BRAVE_C_ */
