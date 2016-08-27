#prevodjenje programa
#(oznaceno s '*' ne prolazi na pinusu, pa je ispod rucno di se to moze)

CC = gcc
CFLAGS = -Wall
LD = gcc
LDFLAGS = -lpthread -lrt

OBJ = obj
BIN = bin

BIBLIOTEKE = brave first_fit
#za lab4b: BIBLIOTEKE = datoteke signali dretve brave first_fit

#* BIBOBJ := $(addsuffix .o, $(addprefix $(OBJ)/, $(BIBLIOTEKE)))
BIBOBJ = obj/brave.o obj/first_fit.o

#* OVISNOSTI := $(addsuffix .d, $(addprefix $(OBJ)/, $(BIBLIOTEKE)))
OVISNOSTI = obj/brave.d obj/first_fit.d

PROG = lab4
#* OVISNOSTI := $(OVISNOSTI) $(addsuffix .d, $(addprefix $(OBJ)/, $(PROG)))
OVISNOSTI = obj/brave.d obj/first_fit.d obj/lab4.d

#umjesto ispisa na zaslon, moze u "dnevnik" ('log file')
#DNEVNIK := $(CURDIR)/$(BIN)/dnevnik.txt
#ili DNEVNIK = dnevnik.txt
#export DNEVNIK #varijabla se stavlja u okolinu za sve programe koje make starta

.PHONY: brisi $(PROG) testdir

#aliasi (na neki nacin, za skraceno pozivanje s 'lab4x' umjesto 'bin/lab4x')
lab4: $(BIN)/lab4

all: $(PROG)

#kako izgradit programe
$(BIN)/%: $(OBJ)/%.o $(BIBOBJ)
	@-if [ ! -d $(BIN) ]; then mkdir -p $(BIN); fi;
	@echo [ld] $@
	@$(LD) $(LDFLAGS) $^ -o $@


#kako izgradit .o datoteke
$(OBJ)/%.o: %.c
	@-if [ ! -d $(OBJ) ]; then mkdir -p $(OBJ); fi;
	@echo [cc] $< ...
	@$(CC) -c $< -o $@ -MMD $(CFLAGS)


testdir:
	@-if [ ! -d $(BIN)/dir1 ]; then mkdir -p $(BIN)/dir1; \
		echo "tekst za prvu datoteku" > $(BIN)/dir1/dat1.txt	;\
		echo "tekst za drugu datoteku" > $(BIN)/dir1/dat2.txt	;\
		echo "tekst za trecu datoteku" > $(BIN)/dir1/dat3.txt	;\
	fi;

start4: lab4
	$(BIN)/lab4 cbt test1 &
	$(BIN)/lab4 ibt test1/test2 &
	$(BIN)/lab4 ebt test1/test2/test3 &
	@sleep 1
	$(BIN)/lab4 p
	@sleep 4
	$(BIN)/lab4 p
	@sleep 5
	$(BIN)/lab4 p
	$(BIN)/lab4 b

#obrisi sve
brisi:
#* ifeq ($(strip $(ZS_IME)),)
#* 	@-if [ -e /dev/shm/lab4ZS ]; then rm -f /dev/shm/lab4ZS ; fi;
#* else
#* 	@-if [ -e /dev/shm/$(ZS_IME) ]; then rm -f /dev/shm/$(ZS_IME); fi;
#* endif
#* 	@-if ( killall -q lab4a lab4b ) ; then 0; fi;
	@-if [ -d $(OBJ) ]; then rm -rf $(OBJ) ; fi;
	@-if [ -d $(BIN) ]; then rm -rf $(BIN) ; fi;


#da se izbjegne nepotrebno brisanje .o datoteka (skripta je previse opca! :) )
#* ne_brisi_sve_objekte_svaki_puta: $(OVISNOSTI:.d=.o)
# $(OVISNOSTI:.d=.o) ce pobrojati sve objekte (umjesto .d stavit ce .o)
#Pogledati: http://www.mail-archive.com/help-make@gnu.org/msg07395.html

# zastavica -MMD ce generirati .d datoteke uz .o datoteke koje ce sadrzavati
# ovisnosti za tu datoteku (gcc to zna generirati sam)
#* -include $(OVISNOSTI)
