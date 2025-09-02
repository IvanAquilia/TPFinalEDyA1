#include "funciones.h"

#include <stdio.h>
#include <string.h>

#include "string_utils.h"
#include "utils.h"
#include "listas.h"
#include "parser.h"

#define LARGO_FUNCIONES 200
#define MAX_RECURSIONES 5000

static void aplicar_funcion_interno(Funcion* funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones);
static void aplicar_base_o_buscar(char* nombre_funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones);
static TipoFuncion str_a_tipo(char* nombre_funcion);

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

void aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones) {
    Lista* list_temp = copiar_lista(lista);
    aplicar_funcion_interno(funcion, list_temp, 1, declaraciones);
    visitar_lista(list_temp);
    printf("\n");
    destruir_lista(list_temp);
}

static void aplicar_funcion_interno(Funcion* funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones) {
    int cantidad_funcion = funcion->tamaño_actual;
    for (int i = 0; i < cantidad_funcion; i++) {
        aplicar_base_o_buscar((char*)funcion->elementos[i], lista, acc_overflow, declaraciones);
    }
}

static void aplicar_base_o_buscar(char* nombre_funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones) {
    if (acc_overflow == MAX_RECURSIONES)
        return;

    TipoFuncion tipo = str_a_tipo(nombre_funcion);

    switch (tipo) {
        case Oi:
            lista_insertar_natural_izquierda(lista, 0);
            break;
        case Od:
            lista_insertar_natural_derecha(lista, 0);
            break;
        case Si:
            if (!lista_vacia(lista)) {
                lista_aumentar_izquierda(lista);
            }
            break;
        case Sd:
            if (!lista_vacia(lista)) {
                lista_aumentar_derecha(lista);
            }
            break;
        case Di:
            if (!lista_vacia(lista)) {
                lista_eliminar_izquierda(lista);
            }
            break;
        case Dd:
            if (!lista_vacia(lista)) {
                lista_eliminar_derecha(lista);
            }
            break;
        case Custom:
            Funcion* funcion = obtener_def_usuario(declaraciones, nombre_funcion, FUNCION);
            aplicar_funcion_interno(funcion, lista, acc_overflow + 1, declaraciones);
            break;
    }
}

static TipoFuncion str_a_tipo(char* nombre_funcion) {
    TipoFuncion tipo = Custom;
    if (strcmp(nombre_funcion, "Oi") == 0)
        tipo = Oi;
    if (strcmp(nombre_funcion, "Od") == 0)
        tipo = Od;
    if (strcmp(nombre_funcion, "Si") == 0)
        tipo = Si;
    if (strcmp(nombre_funcion, "Sd") == 0)
        tipo = Sd;
    if (strcmp(nombre_funcion, "Di") == 0)
        tipo = Di;
    if (strcmp(nombre_funcion, "Dd") == 0)
        tipo = Dd;

    return tipo;
}
