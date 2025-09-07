#include "pila.h"
#include "utils.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>


Pila* pila_crear(FuncionComparadora cmp,
                  FuncionDestructora destruir,
                  FuncionVisitante visitar,
                  FuncionCopia copiar,
                  char* tipo_pila) {

    Pila* pila = malloc(sizeof(Pila));
    assert(pila != NULL);
    char* tipo = malloc(sizeof(char) * strlen(tipo_pila) + 1);
    assert(tipo != NULL);

    strcpy(tipo, tipo_pila);
    pila->glist = glist_crear((FuncionComparadora)cmp,
                              (FuncionDestructora)destruir,
                              (FuncionVisitante)visitar,
                              (FuncionCopia)copiar,
                              tipo);
    free(tipo);
    return pila;
}

void pila_destruir(Pila* pila) {
    if (!pila)
        return;
    glist_destruir(pila->glist);
    free(pila);
}

Pila* copiar_pila(const Pila* pila) {
    Pila* copia = malloc(sizeof(Pila));
    assert(copia != NULL);
    copia->glist = glist_copiar(pila->glist);
    return copia;
}

void pila_push(Pila* pila, void* dato) {
    glist_insertar_inicio(pila->glist, dato);
}

void pila_pop(Pila* pila) {
    // Solo llamare a pop con Pilas no vacias.
    glist_eliminar_inicio(pila->glist);
}

void* pila_top(Pila* pila) {
    return glist_primero(pila->glist);
}

int pila_vacia(Pila* pila) {
    if (pila->glist->head == NULL)
        return 1;
    return 0;
}

unsigned int pila_elementos(Pila* pila) {
    return pila->glist->longitud;
}
