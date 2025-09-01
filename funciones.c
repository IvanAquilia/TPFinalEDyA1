#include "funciones.h"

#include <stdio.h>

#include "string_utils.h"
#include "utils.h"
#include <stdlib.h>

#include "listas.h"
#include "parser.h"

#define LARGO_FUNCIONES 1000

static Funcion* funcion_crear();
static void aplicar_funcion_interno(Funcion* funcion, Lista* lista);

/*
 * Ya recibe string verificada que es una seguidilla de alpha +
 * espacios y con los < > chequeados, no hay problema
 * en modificar la original, se va a destruie luego de
 * todos modos, necesito pasarlo a lista/array
 */
Funcion* strfunc_to_array(char* cadena) {
    Funcion* funcion = funcion_crear();
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
    Declaracion declaracion;
    declaracion.nombre = nombre;
    declaracion.valor = (Funcion*)funcion;
    declaracion.tipo = FUNCION;

    return guardar_declaracion(declaraciones, &declaracion);
}

void aplicar_funcion(Funcion* funcion, Lista* lista) {
    visitar_funcion(funcion);
    visitar_lista(lista);
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

void generar_funciones_base(Declaraciones declaraciones) {
    char* funciones_base[6] = {
        "Oi",
        "Od",
        "Si",
        "Sd",
        "Di",
        "Dd"
    };

    for (int i = 0; i < 6; i++) {
        char* funcion[1] = { funciones_base[i] };
        definir_funcion(funciones_base[i], funcion, declaraciones); // NO ANDA
    }
}

static Funcion* funcion_crear() {
    Funcion* funcion = garray_crear(LARGO_FUNCIONES,
                                    (FuncionComparadora)cmp_str,
                                    (FuncionDestructora)destruir_str,
                                    (FuncionVisitante)visitar_str,
                                    (FuncionCopia)copiar_str);

    return funcion;
}

static void aplicar_funcion_interno(Funcion* funcion, Lista* lista) {
}
