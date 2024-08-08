CFLAGS := -Wall
LDFLAGS := -lncurses

all: main

main: mensagem.o main.o baralho.o crc.o jogo.o
	gcc -o main mensagem.o main.o baralho.o crc.o jogo.o $(LDFLAGS)

mensagem.o: mensagem.h mensagem.c
	gcc -c mensagem.c $(CFLAGS)

jogo.o: jogo.h jogo.c
	gcc -c jogo.c $(CFLAGS)

baralho.o: baralho.h baralho.c
	gcc -c baralho.c $(CFLAGS)

main.o: main.c
	gcc -c main.c $(CFLAGS)

crc.o: crc.h crc.c
	gcc -c crc.c $(CFLAGS)

clean:
	rm -f mensagem.o main.o baralho.o crc.o jogo.o

purge: clean
	rm -f main