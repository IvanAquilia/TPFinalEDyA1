FLAGS = -Wall -Wextra -Werror -std=c99
CC = gcc

programa: main.o glist.o listas.o hash.o utils.o funciones.o parser.o
	$(CC) -o $@ $^ $(FLAGS)

main.o: glist.h parser.h listas.h

parser.o: parser.h utils.h

listas.o: listas.h utils.h

utils.o: utils.h

funciones.o: funciones.h

glist.o:  glist.h

hash.o: hash.h

clean:
	rm *.o
	rm programa

.PHONY = clean
