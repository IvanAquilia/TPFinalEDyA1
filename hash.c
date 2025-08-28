#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash.h"

#define FACTOR_CARGA_MAX 0.7

// ------------------ Crear tabla ------------------
HashTable* hash_crear(int capacidad,
                      FuncionHash hash,
                      FuncionComparadora cmp,
                      FuncionDestructora destruir,
                      FuncionCopia copiar,
                      FuncionVisitante visitar) {
    HashTable* tabla = malloc(sizeof(HashTable));
    assert(tabla != NULL);
    assert(hash != NULL);
    assert(cmp != NULL);
    assert(destruir != NULL);
    assert(copiar != NULL);
    assert(visitar != NULL);

    tabla->capacidad = capacidad;
    tabla->elementos = 0;
    tabla->buckets = calloc(capacidad, sizeof(Entrada*));
    assert(tabla->buckets != NULL);

    tabla->hash = hash;
    tabla->cmp = cmp;
    tabla->destruir = destruir;
    tabla->copiar = copiar;
    tabla->visitar = visitar;

    return tabla;
}

// ------------------ Redimensionar ------------------
static void hash_redimensionar(HashTable* tabla, unsigned int nueva_capacidad) {
    Entrada** nuevos_buckets = calloc(nueva_capacidad, sizeof(Entrada*));
    assert(nuevos_buckets != NULL);

    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            void* dato = tabla->buckets[i]->dato;
            unsigned long h = tabla->hash(dato);
            unsigned long indice = h % nueva_capacidad;
            int insertado = 0;

            for (unsigned int j = 0; j < nueva_capacidad && !insertado; j++) {
                unsigned int pos = (indice + j) % nueva_capacidad;
                if (nuevos_buckets[pos] == NULL) {
                    Entrada* nueva = malloc(sizeof(Entrada));
                    assert(nueva != NULL);

                    nueva->dato = tabla->copiar(dato);
                    nuevos_buckets[pos] = nueva;
                    insertado = 1;
                }
            }
        }
    }

    free(tabla->buckets);
    tabla->buckets = nuevos_buckets;
    tabla->capacidad = nueva_capacidad;
}

// ------------------ Insertar ------------------
int hash_insertar(HashTable* tabla, const void* dato) {
    unsigned int capacidad = tabla->capacidad;
    unsigned long h = tabla->hash(dato);
    unsigned long indice = h % capacidad;

    int insertado = 0, repetido = 0;

    for (unsigned int i = 0; i < tabla->capacidad && !insertado && !repetido; i++) {
        unsigned int pos = (indice + i) % capacidad;
        if (tabla->buckets[pos] == NULL) {
            Entrada* nueva = malloc(sizeof(Entrada));
            assert(nueva != NULL);

            nueva->dato = tabla->copiar(dato);
            tabla->buckets[pos] = nueva;
            tabla->elementos++;
            insertado = 1;
        } else if (tabla->cmp(tabla->buckets[pos]->dato, dato) == 0) {
            repetido = 1;
        }
    }

    if (insertado) {
        double factor = (double)tabla->elementos / capacidad;
        if (factor > FACTOR_CARGA_MAX) {
            hash_redimensionar(tabla, tabla->capacidad * 2);
        }
    }

    return insertado;
}

// ------------------ Buscar ------------------
void* hash_buscar(HashTable* tabla, const void* dato) {
    unsigned int capacidad = tabla->capacidad;
    unsigned long h = tabla->hash(dato);
    unsigned long indice = h % capacidad;
    void* encontrado = NULL;

    // Debido a que no se pueden eliminar elementos, si no se encuentra en el
    // primer slot del linear probing, no lo estará en los siguientes.
    if (tabla->buckets[indice] == NULL)
        return NULL;

    for (unsigned int i = 0; i < tabla->capacidad && !encontrado; i++) {
        unsigned int pos = (indice + i) % capacidad;
        if (tabla->cmp(tabla->buckets[pos]->dato, dato) == 0) {
            encontrado = tabla->buckets[pos]->dato;
        }
    }

    return encontrado;
}


// ------------------ Recorrer ------------------
void hash_recorrer(HashTable* tabla) {
    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            tabla->visitar(tabla->buckets[i]->dato);
        }
    }
}

// ------------------ Destruir ------------------
void hash_destruir(HashTable* tabla) {
    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            tabla->destruir(tabla->buckets[i]->dato);
            free(tabla->buckets[i]);
        }
    }
    free(tabla->buckets);
    free(tabla);
}
