#prevodjenje programa

CC = gcc
CFLAGS = -Wall -D_REENTRANT
LD = gcc
LDFLAGS = -lpthread -lrt -lm

.PHONY: test brisi start3b start3c testdir

test: test_dretve test_procesi
	@echo "***Testiram procese***" ; ./test_procesi
	@echo "***Testiram dretve***" ; ./test_dretve

test_dretve: test_dretve.o dretve_i_procesi.o
	$(LD) $(LDFLAGS) -o $@ $^

test_procesi: test_procesi.o dretve_i_procesi.o
	$(LD) $(LDFLAGS) -o $@ $^

lab3a1: lab3a1.o random_prime.o
	$(LD) $(LDFLAGS) -o $@ $^

lab3a2: lab3a2.o random_prime.o
	$(LD) $(LDFLAGS) -o $@ $^

lab3b: lab3b.o dretve_i_procesi.o signali.o datoteke.o
	$(LD) $(LDFLAGS) -o $@ $^

lab3c: lab3c.o dretve_i_procesi.o signali.o datoteke.o
	$(LD) $(LDFLAGS) -o $@ $^

datoteke.o: datoteke.c datoteke.h
signali.o: signali.c signali.h
dretve_i_procesi.o: dretve_i_procesi.c dretve_i_procesi.h

test_dretve.o: test_dretve.c dretve_i_procesi.h
test_procesi.o: test_procesi.c dretve_i_procesi.h
lab3a1.o: lab3a1.c
lab3a2.o: lab3a2.c
random_prime.o: random_prime.c

lab3b.o: lab3b.c dretve_i_procesi.h signali.h datoteke.h
lab3c.o: lab3c.c dretve_i_procesi.h signali.h datoteke.h logiranje.h

testdir:
	@-if [ ! -d dir1 ]; then mkdir dir1; \
		echo "tekst za prvu datoteku" > dir1/dat1.txt	;\
		echo "tekst za drugu datoteku" > dir1/dat2.txt	;\
		echo "tekst za trecu datoteku" > dir1/dat3.txt	;\
	fi;

start3a1: lab3a1
	./lab3a1 3 5

start3a2: lab3a2
	./lab3a2 3 5

start3b: lab3b testdir
	@echo "***Pokrecem 'lab3b dir1 dir2'***"
	@./lab3b dir1 dir2

# definira se varijabla okoline koja se koristi u 'logiranje.h' kao
# opis mjesta kamo se zapisuje ispis programa
DNEVNIK=dnevnik1

start3c: lab3c testdir
	@echo "***Pokrecem 'lab3c dir1 dir2'***"
	@./lab3c dir1 dir2

brisi:
	@-rm -rf *.o dir1 dir2 test_dretve test_procesi \
		lab3a1 lab3a2 lab3b lab3c dnevnik1
