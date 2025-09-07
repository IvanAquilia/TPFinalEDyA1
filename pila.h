#ifndef PILA_H
#define PILA_H
#include "glist.h"

/*
 * Las Pilas generales son simplemente una GList que utiliza sus datos con filosfia LIFO
 */
typedef struct {
    GList* glist;
} Pila;


/*
 * Explicar campo tipo_pila igual que en glist
 */
Pila* pila_crear(FuncionComparadora cmp,
                  FuncionDestructora destruir,
                  FuncionVisitante visitar,
                  FuncionCopia copiar,
                  char* tipo_pila);

/*
 */
void pila_destruir(Pila* pila);

/*
 */
Pila* copiar_pila(const Pila* pila);

/*
 */
void pila_push(Pila* pila, void* dato);

/*
 */
void pila_pop(Pila* pila);

/*
 */
void* pila_top(Pila* pila);

/*
 */
int pila_vacia(Pila* pila);

/*
 */
unsigned int pila_elementos(Pila* pila);

#endif
