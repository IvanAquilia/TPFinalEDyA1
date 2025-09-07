#include "glist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "string_utils.h"

// ------------------ Crear / Destruir ------------------
GList* glist_crear(FuncionComparadora cmp,
                   FuncionDestructora destruir,
                   FuncionVisitante visitar,
                   FuncionCopia copiar,
                   char* tipo) {
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
    lista->tipo = str_dup(tipo);
    return lista;
}

void glist_destruir(GList* lista) {
    Nodo* actual = lista->head;
    while (actual != NULL) {
        Nodo* temp = actual;
        actual = actual->sig;
        lista->destruir(temp->dato);

        free(temp);
    }
    free(lista->tipo);
    free(lista);
}

// ------------------ Insertar ------------------
void glist_insertar_inicio(GList* lista, void* dato) {
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

void glist_insertar_final(GList* lista, void* dato) {
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
int glist_eliminar_inicio(GList* lista) {
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

int glist_eliminar_final(GList* lista) {
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
void* glist_primero(GList* lista) {
    if (lista->head == NULL)
        return NULL;
    return lista->head->dato;
}

void* glist_ultimo(GList* lista) {
    if (lista->tail == NULL)
        return NULL;
    return lista->tail->dato;
}

// ------------------ Utilidades ------------------
void glist_imprimir(const GList* lista) {
    Nodo* actual = lista->head;
    while (actual != NULL) {
        lista->visitar(actual->dato);
        actual = actual->sig;
        if (actual != NULL)
            printf(", ");
    }
}

GList* glist_copiar(const GList* original) {
    GList* copia = glist_crear(original->cmp,
                               original->destruir,
                               original->visitar,
                               original->copiar,
                               original->tipo);

    Nodo* actual = original->head;
    while (actual != NULL) {
        glist_insertar_final(copia, actual->dato);
        actual = actual->sig;
    }
    return copia;
}

int glist_iguales(GList* lista1, GList* lista2) {
    int iguales = 1;
    if (lista1->longitud == lista2->longitud && strcmp(lista1->tipo, lista2->tipo) == 0) {
        Nodo* actual_l1 = lista1->head;
        Nodo* actual_l2 = lista2->head;
        while (actual_l1 != NULL && actual_l2 != NULL && iguales) {
            if (lista1->cmp(actual_l1->dato, actual_l2->dato) != 0)
                iguales = 0;
            actual_l1 = actual_l1->sig;
            actual_l2 = actual_l2->sig;
        }
    } else {
        iguales = 0;
    }

    return iguales;
}
