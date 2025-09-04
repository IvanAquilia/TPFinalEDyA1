#include "pila.h"
#include "utils.h"
#include <stdlib.h>
#include <assert.h>


Pila* pila_crear() {
    Pila* pila = malloc(sizeof(Pila));
    assert(pila != NULL);
    pila->glist = glist_crear((FuncionComparadora)cmp_uint,
                              (FuncionDestructora)destruir_uint,
                              (FuncionVisitante)visitar_uint,
                              (FuncionCopia)copiar_uint);
    return pila;
}

void pila_destruir(Pila* pila) {
    if (!pila)
        return;
    glist_destruir(pila->glist);
    free(pila);
}

void pila_push(Pila* pila, unsigned int dato) {
    unsigned int temp = dato;
    glist_insertar_inicio(pila->glist, &temp);
}

void pila_pop(Pila* pila) {
    // Solo llamare a pop con Pilas no vacias.
    glist_eliminar_inicio(pila->glist);
}

unsigned int* pila_top(Pila* pila) {
    unsigned int* dato = (unsigned int*)glist_primero(pila->glist);
    return dato;
}

int pila_vacia(Pila* pila) {
    if (pila->glist->head == NULL)
        return 1;
    return 0;
}
