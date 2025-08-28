#include "funciones.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"

/*
 * Ya recibe string verificada que es una seguidilla de alpha +
 * espacios y con los < > chequeados, no hay problema
 * en modificar la original, se va a destruie luego de
 * todos modos, necesito pasarlo a lista/array
 */
Funcion strfunc_to_array(char* cadena) {
    Funcion funcion = funcion_crear();
    char* cursor = cadena;
    while (*cursor) {
        avanzar_hasta_noespacio(&cursor);
        if (*cursor == '<' || *cursor == '>') {
            // Uso memoria estatica, total luego en
            // 'componer_funcion' guardo una copia con strcpy
            char simbolo_repeticion[2] = { *cursor, '\0' };

            componer_funcion(funcion, simbolo_repeticion);
            cursor++;
        } else if (*cursor) {
            char* nombre = cursor;
            avanzar_hasta_espacio_o_repeticion(&cursor);
            char temp = *cursor;
            *cursor = '\0';
            componer_funcion(funcion, nombre);

            // Restauro el carácter que habia previamente antes del '\0'
            // por si era algun '<' o '>' a tener en cuenta mas tarde
            *cursor = temp;
        }
    }

    return funcion;
}

Funcion funcion_crear() {
    Funcion funcion = malloc(sizeof(struct _Funcion));
    char** definicion = malloc(100 * sizeof(char*));
    assert(funcion != NULL);
    assert(definicion != NULL);

    funcion->definicion = definicion;
    funcion->capacidad = 100;
    funcion->cantidad_actual = 0;
    return funcion;
}

static void componer_funcion(Funcion funcion, char* nombre) {
    if (funcion->cantidad_actual + 1 == funcion->capacidad)
        extender_largo_definicion(funcion);

    size_t largo = strlen(nombre);
    funcion->definicion[funcion->cantidad_actual] = malloc(largo * sizeof(char) + 1);
    assert(funcion->definicion[funcion->cantidad_actual] != NULL);
    strcpy(funcion->definicion[funcion->cantidad_actual], nombre);
    funcion->cantidad_actual++;
}

static void extender_largo_definicion(Funcion funcion) {
    char** temp = realloc(funcion->definicion, (funcion->capacidad * 2) * sizeof(char*));
    assert(temp != NULL);
    funcion->definicion = temp;
}

void funcion_mostrar(Funcion funcion) {
    int cant = funcion->cantidad_actual;
    for (int i = 0; i < cant; i++) {
        printf("Componente %d: %s \n", i, funcion->definicion[i]);
    }
}