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
 * Crea una pila generica con las funciones correspondientes al tipo de dato a almecenar.
 * El argumento tipo_pila se guarda internamente en el mismo campo pero de la GList que de fondo hacer funcionar
 * a la pila. Sirve para las comparaciones seguras.
 */
Pila* pila_crear(FuncionComparadora cmp,
    FuncionDestructora destruir,
    FuncionVisitante visitar,
    FuncionCopia copiar,
    char* tipo_pila);

/*
 * Destruye una pila.
 */
void pila_destruir(Pila* pila);

/*
 * Realiza una copia profunda de la pila.
 */
Pila* copiar_pila(const Pila* pila);

/*
 * Apila un elemento
 */
void pila_push(Pila* pila, void* dato);

/*
 * Elimina el ultimo elemento de la pila
 */
void pila_pop(Pila* pila);

/*
 * Devuelve un puntero al ultimo elemento de la pila, no modifica la pila.
 */
void* pila_top(Pila* pila);

/*
 * Devuelve 1 si la pila no tiene elementos, sino 0
 */
int pila_vacia(Pila* pila);

/*
 * Devuele la cantidad de elementos de la pila.
 */
unsigned int pila_elementos(Pila* pila);

#endif
