/*! datoteke.h
 *
 * Sucelje za rad s datotekama
 */



char **dohvati_datoteke ( const char *direktorij, int *broj );

int usporedi ( const char *ime1, const char *ime2 );

int kopiraj_datoteku ( const char *ime1, const char *ime2 );

int vrijeme_zadnje_promjene ( const char *ime, char *vrijeme );

void *pozovi_kopiraj_datoteku ( int id, void *pparam, int iparam );

void dodaj_posao_kopiranja ( void *dretve, char *ime1, char *ime2 );

