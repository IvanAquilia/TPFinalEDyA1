FLAGS = -Wall -Wextra -Werror -O2 -std=c99 -lm
CC = gcc

programa: main.o parser.o listas.o funciones.o utils.o string_utils.o glist.o listas.o hash.o garray.o pila.o
	$(CC) -o $@ $^ $(FLAGS)

main.o: glist.h parser.h listas.h

parser.o: parser.h utils.h

listas.o: listas.h utils.h

funciones.o: funciones.h

pila.o: pila.h

utils.o: utils.h

string_utils.o: string_utils.h

glist.o:  glist.h

hash.o: hash.h

garray.o: garray.h

clean:
	rm *.o
	rm programa

.PHONY = clean
