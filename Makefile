FLAGS = -Wall -Wextra -Werror -std=c99
CC = gcc

programa: main.o parser.o listas.o funciones.o utils.o string_utils.o glist.o listas.o hash.o garray.o
	$(CC) -o $@ $^ $(FLAGS)

main.o: glist.h parser.h listas.h

parser.o: parser.h utils.h

listas.o: listas.h utils.h

funciones.o: funciones.h

utils.o: utils.h

string_utils.o: string_utils.h

glist.o:  glist.h

hash.o: hash.h

garray.o: garray.h

clean:
	rm *.o
	rm programa

.PHONY = clean
