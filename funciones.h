#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "glist.h"

/*
 * Cada funcion es un array dinamico de los nombres (char*) de las funciones que la compone en orden.
 * Ej. : f1: ["0d", "Mi", "<Dd>", "Si"]
 */
struct _Funcion {
    char** definicion;
    int capacidad;
    int cantidad_actual;
};

typedef struct _Funcion* Funcion;

Funcion strfunc_to_array(char* cadena);

Funcion funcion_crear();

static void componer_funcion(Funcion funcion, char* nombre);

int cmp_str(const char* a, const char* b);

void destruir_str(char* dato);

char* copiar_str(const char* dato);

void visitar_str(const char* dato);

static void extender_largo_definicion(Funcion funcion);

void funcion_mostrar(Funcion funcion);

#endif
