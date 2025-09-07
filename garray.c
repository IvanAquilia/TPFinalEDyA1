#include "garray.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CAPACIDAD_INICIAL 200
#define FACTOR_CRECIMIENTO 2

/*
 * Redimensiona el array con re-alloc hacia la nueva capacidad.
 */
static void garray_redimensionar(GArray* array, unsigned int nueva_capacidad);

// ------------------ Crear / Destruir ------------------
GArray* garray_crear(unsigned int capacidad_inicial,
                    FuncionComparadora cmp,
                    FuncionDestructora destruir,
                    FuncionVisitante visitar,
                    FuncionCopia copiar) {

    GArray* array = malloc(sizeof(GArray));
    assert(array != NULL);
    assert(cmp != NULL);
    assert(destruir != NULL);
    assert(visitar != NULL);
    assert(copiar != NULL);

    unsigned int capacidad = (capacidad_inicial > 0) ? capacidad_inicial : CAPACIDAD_INICIAL;
    array->elementos = malloc(capacidad * sizeof(void*));
    assert(array->elementos != NULL);

    array->capacidad = capacidad;
    array->tamaño_actual = 0;
    array->cmp = cmp;
    array->destruir = destruir;
    array->visitar = visitar;
    array->copiar = copiar;

    return array;
}

void garray_destruir(GArray* array) {
    if (array == NULL) return;

    for (unsigned int i = 0; i < array->tamaño_actual; i++) {
        array->destruir(array->elementos[i]);
    }

    free(array->elementos);
    free(array);
}

// ------------------ Insertar ------------------
void garray_insertar(GArray* array, void* dato) {
    if (array->tamaño_actual >= array->capacidad) {
        garray_redimensionar(array, array->capacidad * FACTOR_CRECIMIENTO);
    }

    array->elementos[array->tamaño_actual] = array->copiar(dato);
    array->tamaño_actual++;
}

// ------------------ Eliminar ------------------
int garray_eliminar(GArray* array) {
    if (array->tamaño_actual == 0) {
        return 0;
    }

    array->destruir(array->elementos[array->tamaño_actual - 1]);
    array->tamaño_actual--;
    return 1;
}

// ------------------ Consultas ------------------
void* garray_obtener(GArray* array, unsigned int posicion) {
    if (posicion >= array->tamaño_actual) {
        return NULL;
    }
    return array->elementos[posicion];
}

// ------------------ Utilidades ------------------
void garray_imprimir(const GArray* array) {
    for (unsigned int i = 0; i < array->tamaño_actual; i++) {
        array->visitar(array->elementos[i]);
    }
}

GArray* garray_copiar(const GArray* original) {
    GArray* copia = garray_crear(original->capacidad,
                                 original->cmp,
                                 original->destruir,
                                 original->visitar,
                                 original->copiar);

    for (unsigned int i = 0; i < original->tamaño_actual; i++) {
        garray_insertar(copia, original->elementos[i]);
    }

    return copia;
}

// ------------------ Utilidades internas ------------------
static void garray_redimensionar(GArray* array, unsigned int nueva_capacidad) {
    void** nuevos_elementos = realloc(array->elementos, nueva_capacidad * sizeof(void*));
    assert(nuevos_elementos != NULL);

    array->elementos = nuevos_elementos;
    array->capacidad = nueva_capacidad;
}
