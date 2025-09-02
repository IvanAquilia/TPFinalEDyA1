#include "funciones.h"

#include <stdio.h>

#include "string_utils.h"
#include "utils.h"
#include "listas.h"
#include "parser.h"

#define LARGO_FUNCIONES 200

static void aplicar_funcion_interno(Funcion* funcion, Lista* lista);

/*
 * Ya recibe string verificada que es una seguidilla de alpha +
 * espacios y con los < > chequeados, no hay problema
 * en modificar la original, se va a destruie luego de
 * todos modos, necesito pasarlo a lista/array
 */
Funcion* strfunc_to_array(char* cadena, Declaraciones declaraciones) {
    Funcion* funcion = funcion_crear();
    char* cursor = cadena;
    int funcion_inexistente = 0;
    while (*cursor && !funcion_inexistente) {
        avanzar_hasta_noespacio(&cursor);

        if (*cursor == '<' || *cursor == '>') {
            char simbolo_repeticion[2] = { *cursor, '\0' };

            componer_funcion(funcion, simbolo_repeticion);
            cursor++;
        } else if (*cursor) {
            char* nombre = cursor;
            avanzar_hasta_espacio_o_repeticion(&cursor);
            char temp = *cursor;
            *cursor = '\0';

            // Chequeo que la funcion a componer exista
            if (obtener_def_usuario(declaraciones, nombre, FUNCION))
                componer_funcion(funcion, nombre);
            else
                funcion_inexistente = 1;

            // Devuelvo el valor previo a poner '\0' para no salir
            // del while (*cursor == '\0') y moverme hasta la siguiente funcion.
            *cursor = temp;
        }
    }

    if (funcion_inexistente) {
        destruir_funcion(funcion);
        return NULL;
    }
    return funcion;
}

Funcion* funcion_crear() {
    Funcion* funcion = garray_crear(LARGO_FUNCIONES,
                                    (FuncionComparadora)cmp_str,
                                    (FuncionDestructora)destruir_str,
                                    (FuncionVisitante)visitar_str,
                                    (FuncionCopia)copiar_str);

    return funcion;
}


void componer_funcion(Funcion* funcion, char* nombre) {
    garray_insertar(funcion, nombre);
}

void destruir_funcion(Funcion* funcion) {
    garray_destruir(funcion);
}

Funcion* copiar_funcion(const Funcion* funcion) {
    return garray_copiar(funcion);
}

void visitar_funcion(const Funcion* funcion) {
    garray_imprimir(funcion);
}

int definir_funcion(char* nombre, void* funcion, Declaraciones declaraciones) {
    // Creo mi declaracion temporal, al salir de aca se destruye,
    // luego se destruye tambien la funcion en el main.c
    Declaracion declaracion;
    declaracion.nombre = nombre;
    declaracion.valor = (Funcion*)funcion;
    declaracion.tipo = FUNCION;

    return guardar_declaracion(declaraciones, &declaracion);
}

int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
                            char* nombre_funcion, char* string_lista,
                            int in_place, Declaraciones declaraciones) {
    *funcion = (Funcion*)obtener_def_usuario(declaraciones, nombre_funcion, FUNCION);

    if (in_place && verificar_lista(string_lista))
        *lista = strlist_to_lista(string_lista);
    else
        *lista = (Lista*)obtener_def_usuario(declaraciones, string_lista, LISTA);

    if (*funcion && *lista)
        return 1;
    return 0;
}

void aplicar_funcion(Funcion* funcion, Lista* lista) {
    visitar_funcion(funcion);
    visitar_lista(lista);
}

static void aplicar_funcion_interno(Funcion* funcion, Lista* lista) {
}
