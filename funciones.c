#include "funciones.h"
#include "string_utils.h"
#include "utils.h"

#define LARGO_FUNCIONES 1000

static Funcion* funcion_crear();

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
    garray_insertar(funcion, (void*)nombre);
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

static Funcion* funcion_crear() {
    Funcion* funcion = garray_crear(LARGO_FUNCIONES,
                                    (FuncionComparadora)cmp_str,
                                    (FuncionDestructora)destruir_str,
                                    (FuncionVisitante)visitar_str,
                                    (FuncionCopia)copiar_str);

    return funcion;
}
