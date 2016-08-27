#prevodjenje programa

CC = gcc
CFLAGS = -Wall
LD = gcc
LDFLAGS = -lrt

.PHONY: testdir pokreni brisi

lab2: signali.o lab2.o
	$(LD) $(LDFLAGS) signali.o lab2.o -o lab2

lab2a: signali.o lab2a.o
	$(LD) $(LDFLAGS) signali.o lab2a.o -o lab2a

lab2b: signali.o lab2b.o
	$(LD) $(LDFLAGS) signali.o lab2b.o -o lab2b

lab2c: signali.o datoteke.o lab2c.o
	$(LD) $(LDFLAGS) signali.o datoteke.o lab2c.o -o lab2c


datoteke.o: datoteke.c datoteke.h
	$(CC) $(CFLAGS) -c datoteke.c

signali.o: signali.c signali.h
	$(CC) $(CFLAGS) -c signali.c

lab2.o: lab2.c signali.h
	$(CC) $(CFLAGS) -c lab2.c

lab2a.o: lab2a.c signali.h
	$(CC) $(CFLAGS) -c lab2a.c

lab2b.o: lab2b.c signali.h
	$(CC) $(CFLAGS) -c lab2b.c

lab2c.o: lab2c.c signali.h datoteke.h
	$(CC) $(CFLAGS) -c lab2c.c


testdir:
	@-if [ ! -d dir1 ]; then mkdir dir1; \
		echo "tekst za prvu datoteku" > dir1/dat1.txt  ;\
		echo "tekst za drugu datoteku" > dir1/dat2.txt ;\
		echo "tekst za trecu datoteku" > dir1/dat3.txt ;\
	fi;

pokreni: testdir lab2c lab2b
	@echo "Primjer pokretanja (u drugoj konzoli pokrenuti ./lab2b pid):"
	./lab2c dir1 dir2

sve all: lab2 lab2a lab2b lab2c

brisi obrisi:
	rm -rf *.o lab2 lab2a lab2b lab2c dir*
