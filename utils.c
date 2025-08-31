#include "utils.h"
#include "glist.h"
#include "listas.h"
#include "funciones.h"
#include "string_utils.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define CANTIDAD_DECLARACIONES 10000

static int guardar_declaracion(Declaraciones declaraciones, Declaracion* declaracion);

// -------------- Funciones de comparacion, copia... etc. pasadas a las estructuras -------------- //

// ----------- STRINGS
int cmp_str(const char* a, const char* b) {
    return strcmp(a, b);
}

void destruir_str(char* dato) {
    free(dato);
}

char* copiar_str(const char* dato) {
    char* nuevo = malloc(sizeof(char) * strlen(dato) + 1);
    assert(nuevo != NULL);
    strcpy(nuevo, dato);
    return nuevo;
}

void visitar_str(const char* dato) {
    printf("%s ", dato);
}

// ----------- ENTEROS
int cmp_int(const int* a, const int* b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

void destruir_int(int* entero) {
    free(entero);
}

int* copiar_int(const int* entero) {
    int* nuevo = malloc(sizeof(int));
    *nuevo = *entero;
    return nuevo;
}

void visitar_int(const int* entero) {
    printf("%d ", *entero);
}

// ----------- DECLARACIONES
Declaracion* declaracion_crear(TipoDeclaracion tipo, char* nombre, void* valor) {
    Declaracion* declaracion = malloc(sizeof(Declaracion));
    assert(declaracion != NULL);

    declaracion->nombre = malloc(strlen(nombre) + 1);
    assert(declaracion->nombre != NULL);
    strcpy(declaracion->nombre, nombre);
    declaracion->tipo = tipo;

    if (declaracion->tipo == LISTA) {
        declaracion->valor = (void*)copiar_lista((Lista*)valor);
    } else {
        declaracion->valor = (void*)copiar_funcion((Funcion*)valor);;
    }

    return declaracion;
}

int cmp_declaracion(const Declaracion* a, const Declaracion* b) {
    if (a->tipo == b->tipo) {
        int nombres_iguales = strcmp(a->nombre, b->nombre);
        return nombres_iguales;
    }
    return -1;
}

void destruir_declaracion(Declaracion* declaracion) {
    free(declaracion->nombre);
    if (declaracion->tipo == LISTA)
        destruir_lista((Lista*)declaracion->valor);
    else
        destruir_funcion((Funcion*)declaracion->valor);

    free(declaracion);
}

Declaracion* copiar_declaracion(const Declaracion* declaracion) {
    Declaracion* nueva_declaracion = malloc(sizeof(Declaracion));
    assert(nueva_declaracion != NULL);

    nueva_declaracion->nombre = malloc(strlen(declaracion->nombre) + 1);
    assert(nueva_declaracion->nombre != NULL);
    strcpy(nueva_declaracion->nombre, declaracion->nombre);

    nueva_declaracion->tipo = declaracion->tipo;
    if (declaracion->tipo == LISTA) {
        nueva_declaracion->valor = copiar_lista(declaracion->valor);
    } else {
        nueva_declaracion->valor = (void*)copiar_funcion(declaracion->valor);;
    }

    return nueva_declaracion;
}

void visitar_declaracion(const Declaracion* declaracion) {
    if (declaracion->tipo == LISTA) {
        printf("LISTA nombrada '%s': ", declaracion->nombre);
        visitar_lista(declaracion->valor);
        printf("\n");
    } else {
        printf("FUNCION nombrada '%s': ", declaracion->nombre);
        visitar_funcion(declaracion->valor);
        printf("\n");
    }
}

Declaraciones declaraciones_crear() {
    Declaraciones declaraciones = tabla_hash_crear(CANTIDAD_DECLARACIONES,
                                                    (FuncionHash)hash_declaracion,
                                                    (FuncionComparadora)cmp_declaracion,
                                                    (FuncionDestructora)destruir_declaracion,
                                                    (FuncionCopia)copiar_declaracion,
                                                    (FuncionVisitante)visitar_declaracion);
    return declaraciones;
}

void hashear_y_manejar_output(Declaraciones declaraciones, Declaracion* declaracion) {
    int insertado = guardar_declaracion(declaraciones, declaracion);

    if (!insertado)
        if (declaracion->tipo == LISTA)
            printf("ERROR: ya existe una lista con nombre '%s'\n", declaracion->nombre);
        else
            printf("ERROR: ya existe una funcion con nombre '%s'\n", declaracion->nombre);
    else
        if (declaracion->tipo == LISTA)
            printf("Lista '%s' definida con exito\n", declaracion->nombre);
        else
            printf("Funcion '%s' definida con exito\n", declaracion->nombre);

    destruir_declaracion(declaracion);
}

static int guardar_declaracion(Declaraciones declaraciones, Declaracion* declaracion) {
    return tabla_hash_insertar(declaraciones, (void*)declaracion);
}

