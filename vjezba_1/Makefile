#prevodjenje programa

CC = gcc
CFLAGS = -Wall
LD = gcc
LDFLAGS =

.PHONY: testdir pokreni brisi

lab1: datoteke.o lab1.o
	$(LD) $(LDFLAGS) datoteke.o lab1.o -o lab1

#ili samo:
#	$(LD) $(LDFLAGS) $^ -o $@
# '$@' se zamjenjuje sa 'lab1', a '$^' sa 'datoteke.o lab1.o' (sve iza ':')
# ovaj drugi redak ($(LD)...) i nije potreban, make 'zna' to i sam napraviti
# (ali "noviji", ne ovaj na pinusu)

#zbog .h datoteke potrebne su i definicije ovisnosti
datoteke.o: datoteke.c datoteke.h
	$(CC) $(CFLAGS) -c datoteke.c

#ili samo:
#	$(CC) $(CFLAGS) -c $< -o $@
# '$<' se zamjenjuje s prvim izrazom iza ':', tj. sa 'datoteke.c'
# ovaj drugi redak ($(CC)...) i nije potreban, make 'zna' to i sam napraviti

lab1.o: lab1.c datoteke.h


listaj: izlistaj.c
	$(CC) $(CFLAGS) -c izlistaj.c -o listaj.o
	$(LD) $(LDFLAGS) listaj.o -o listaj

kopiraj: kopiraj.c
	$(CC) $(CFLAGS) -c kopiraj.c
	$(LD) $(LDFLAGS) kopiraj.o -o kopiraj
# ove zadnje dvije linije ovdje ('kopiraj') i nisu potrebne obzirom da je
# 'target' istog imena kao i izvorna datoteka

testdir:
	@-if [ ! -d dir1 ]; then mkdir dir1; \
		echo "tekst za prvu datoteku" > dir1/dat1.txt  ;\
		echo "tekst za drugu datoteku" > dir1/dat2.txt ;\
		echo "tekst za trecu datoteku" > dir1/dat3.txt ;\
	fi;

test: listaj kopiraj testdir
	./listaj dir1
	./kopiraj dir1 dir2
	./listaj dir2


pokreni: testdir lab1 listaj
	@echo Primjer pokretanja:
	./lab1 dir1 dir2
	./listaj dir1
	./listaj dir2
	@echo Slijedi promjena u dat2.txt:
	@sleep 3
	cat dir1/dat1.txt >> dir1/dat2.txt
	./lab1 dir1 dir2
	./listaj dir1
	./listaj dir2

brisi:
	rm -rf *.o lab1 listaj kopiraj dir*

# pokretanje prevodjenja: 'make *sto*' (npr. 'make lab1')
# pokretanje programa: 'make test' i 'make pokreni'
# brisanje stvorenih datoteka (.o) i direktorija: 'make brisi'
