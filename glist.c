#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "glist.h"

// ------------------ Crear / Destruir ------------------
GList* lista_crear(FuncionComparadora cmp,
                   FuncionDestructora destruir,
                   FuncionVisitante visitar,
                   FuncionCopia copiar) {
    GList* lista = malloc(sizeof(GList));
    assert(lista != NULL);
    assert(cmp != NULL);
    assert(destruir != NULL);
    assert(visitar != NULL);
    assert(copiar != NULL);

    lista->head = NULL;
    lista->tail = NULL;
    lista->longitud = 0;
    lista->cmp = cmp;
    lista->destruir = destruir;
    lista->visitar = visitar;
    lista->copiar = copiar;
    return lista;
}

void lista_destruir(GList* lista) {
    Nodo* actual = lista->head;
    while (actual != NULL) {
        Nodo* temp = actual;
        actual = actual->sig;
        lista->destruir(temp->dato);

        free(temp);
    }
    free(lista);
}

// ------------------ Insertar ------------------
void lista_insertar_inicio(GList* lista, void* dato) {
    Nodo* nuevo = malloc(sizeof(Nodo));
    assert(nuevo != NULL);
    nuevo->dato = lista->copiar(dato);
    nuevo->sig = lista->head;
    nuevo->ant = NULL;

    if (lista->head != NULL) {
        lista->head->ant = nuevo;
    } else {
        lista->tail = nuevo; // lista vacía
    }

    lista->head = nuevo;
    lista->longitud++;
}

void lista_insertar_final(GList* lista, void* dato) {
    Nodo* nuevo = malloc(sizeof(Nodo));
    assert(nuevo != NULL);
    nuevo->dato = lista->copiar(dato);
    nuevo->ant = lista->tail;
    nuevo->sig = NULL;

    if (lista->tail != NULL) {
        lista->tail->sig = nuevo;
    } else {
        lista->head = nuevo; // lista vacía
    }

    lista->tail = nuevo;
    lista->longitud++;
}

// ------------------ Eliminar ------------------
int lista_eliminar_inicio(GList* lista) {
    if (lista->head == NULL)
        return 0;

    Nodo* temp = lista->head;
    lista->head = lista->head->sig;

    if (lista->head != NULL) {
        lista->head->ant = NULL;
    } else {
        lista->tail = NULL; // quedó vacía
    }

    lista->destruir(temp->dato);

    free(temp);
    lista->longitud--;
    return 1;
}

int lista_eliminar_final(GList* lista) {
    if (lista->tail == NULL)
        return 0;

    Nodo* temp = lista->tail;
    lista->tail = lista->tail->ant;

    if (lista->tail != NULL) {
        lista->tail->sig = NULL;
    } else {
        lista->head = NULL; // quedó vacía
    }

    lista->destruir(temp->dato);

    free(temp);
    lista->longitud--;
    return 1;
}

// ------------------ Consultas ------------------
void* lista_primero(GList* lista) {
    if (lista->head == NULL)
        return NULL;
    return lista->head->dato;
}

void* lista_ultimo(GList* lista) {
    if (lista->tail == NULL)
        return NULL;
    return lista->tail->dato;
}

// ------------------ Buscar ------------------
void* lista_buscar(GList* lista, void* dato) {
    Nodo* actual = lista->head;
    while (actual != NULL) {
        if (lista->cmp(actual->dato, dato) == 0) {
            return actual->dato;
        }
        actual = actual->sig;
    }
    return NULL;
}

// ------------------ Utilidades ------------------
void lista_imprimir(GList* lista) {
    Nodo* actual = lista->head;
    while (actual != NULL) {
        lista->visitar(actual->dato);
        actual = actual->sig;
    }
}

GList* lista_copiar(GList* original) {
    GList* copia = lista_crear(original->cmp,
                               original->destruir,
                               original->visitar,
                               original->copiar);

    Nodo* actual = original->head;
    while (actual != NULL) {
        lista_insertar_final(copia, actual->dato);
        actual = actual->sig;
    }
    return copia;
}
