/*! signali.h
 *
 * Sucelja funkcija za prihvat signala
 */

int registriraj_signal ( int sig, const void *func, void *f_prije );
int blokiraj_signal ( int sig );
int odblokiraj_signal ( int sig );
int posalji_signal ( int sig, int kome, int param );

