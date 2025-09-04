#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "utils.h"

#define FACTOR_CARGA_MAX 0.7

static void tabla_hash_redimensionar(HashTable* tabla, unsigned int nueva_capacidad);

// ------------------ Crear / Destruir ------------------
HashTable* tabla_hash_crear(unsigned int capacidad,
                            FuncionHash hash,
                            FuncionComparadora cmp_clave,
                            FuncionDestructora destruir,
                            FuncionDestructora destruir_clave,
                            FuncionCopia copiar,
                            FuncionCopia copiar_clave,
                            FuncionVisitante visitar) {
    HashTable* tabla = malloc(sizeof(HashTable));
    assert(tabla != NULL);
    assert(hash != NULL);

    assert(cmp_clave != NULL);
    assert(destruir != NULL); assert(destruir_clave != NULL);
    assert(copiar != NULL); assert(copiar_clave != NULL);

    assert(visitar != NULL);

    tabla->capacidad = capacidad;
    tabla->elementos = 0;
    tabla->buckets = calloc(capacidad, sizeof(Entrada*));
    assert(tabla->buckets != NULL);

    tabla->hash = hash;
    tabla->cmp_clave = cmp_clave;

    tabla->destruir = destruir;
    tabla->destruir_clave = destruir_clave;
    tabla->copiar = copiar;
    tabla->copiar_clave = copiar_clave;

    tabla->visitar = visitar;

    return tabla;
}

void tabla_hash_destruir(HashTable* tabla) {
    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            tabla->destruir(tabla->buckets[i]->dato);
            tabla->destruir_clave(tabla->buckets[i]->clave);
            free(tabla->buckets[i]);
        }
    }
    free(tabla->buckets);
    free(tabla);
}

// ------------------ Insertar ------------------
int tabla_hash_insertar(HashTable* tabla, const void* clave, const void* dato) {
    if (dato == NULL || clave == NULL)
        return 0;

    unsigned int capacidad = tabla->capacidad;
    unsigned long h = tabla->hash(clave);
    unsigned long indice = h % capacidad;

    int insertado = 0, repetido = 0;

    for (unsigned int i = 0; i < capacidad && !insertado && !repetido; i++) {
        unsigned int pos = (indice + i) % capacidad;
        if (tabla->buckets[pos] == NULL) {
            Entrada* nueva = malloc(sizeof(Entrada));
            assert(nueva != NULL);

            nueva->dato = tabla->copiar(dato);
            nueva->clave = tabla->copiar_clave(clave);
            tabla->buckets[pos] = nueva;
            tabla->elementos++;
            insertado = 1;
        } else if (tabla->cmp_clave(tabla->buckets[pos]->clave, clave) == 0) {
            repetido = 1;
        }
    }

    if (insertado) {
        double factor = (double)tabla->elementos / capacidad;
        if (factor > FACTOR_CARGA_MAX) {
            unsigned int nueva_capacidad = siguiente_primo(capacidad * 2);
            tabla_hash_redimensionar(tabla, nueva_capacidad);
        }
    }

    return insertado;
}

// ------------------ Buscar ------------------
void* tabla_hash_buscar(HashTable* tabla, const void* clave) {
    if (clave == NULL)
        return NULL;

    unsigned int capacidad = tabla->capacidad;
    unsigned long h = tabla->hash(clave);
    unsigned long indice = h % capacidad;
    void* encontrado = NULL;

    // Debido a que no se pueden eliminar elementos, si no se encuentra en el
    // primer slot del linear probing, no lo estará en los siguientes.
    if (tabla->buckets[indice] == NULL)
        return NULL;

    for (unsigned int i = 0; i < tabla->capacidad && !encontrado; i++) {
        unsigned int pos = (indice + i) % capacidad;
        if (tabla->cmp_clave(tabla->buckets[pos]->clave, clave) == 0) {
            encontrado = tabla->buckets[pos]->dato;
        }
    }

    return encontrado;
}

// ------------------ Utilidades ------------------
void tabla_hash_recorrer(HashTable* tabla) {
    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            tabla->visitar(tabla->buckets[i]->dato);
        }
    }
}

// ------------------ Utilidades internas ------------------
static void tabla_hash_redimensionar(HashTable* tabla, unsigned int nueva_capacidad) {
    Entrada** nuevos_buckets = calloc(nueva_capacidad, sizeof(Entrada*));
    assert(nuevos_buckets != NULL);

    for (unsigned int i = 0; i < tabla->capacidad; i++) {
        if (tabla->buckets[i] != NULL) {
            void* dato = tabla->buckets[i]->dato;
            void* clave = tabla->buckets[i]->clave;
            unsigned long h = tabla->hash(clave);
            unsigned long indice = h % nueva_capacidad;
            int insertado = 0;

            // For de linear probing para cada elemento de la nueva tabla
            for (unsigned int j = 0; j < nueva_capacidad && !insertado; j++) {
                unsigned int pos = (indice + j) % nueva_capacidad;
                if (nuevos_buckets[pos] == NULL) {
                    Entrada* nueva = malloc(sizeof(Entrada));
                    assert(nueva != NULL);

                    nueva->clave = tabla->copiar_clave(clave);
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
