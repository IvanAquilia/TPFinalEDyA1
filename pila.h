#ifndef PILA_H
#define PILA_H
#include "glist.h"

/*
 * Las pilas de naturales son simplemente una GList que utiliza naturales con filosfia LIFO
 */
typedef struct {
    GList* glist;
} Pila;


/*
 */
Pila* pila_crear();

/*
 */
void pila_destruir(Pila* pila);

/*
 */
void pila_push(Pila* pila, unsigned int dato);

/*
 */
void pila_pop(Pila* pila);

/*
 */
unsigned int* pila_top(Pila* pila);

/*
 */
int pila_vacia(Pila* pila);

#endif
