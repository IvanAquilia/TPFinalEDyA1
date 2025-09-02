#include "listas.h"
#include "string_utils.h"
#include "utils.h"
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Lista* lista_crear();

/*
 * Ya recibe lista verificada que es minimo [] y tiene digitos validos dentro,
 * no importa modificar la original aca, necesito pasarla a lista total
 * despues se pisa el buffer e nuevo.
 */
Lista* strlist_to_lista(char* cadena) {
    size_t len = strlen(cadena);
    Lista* lista = lista_crear();

    int invalido = 0;
    if (len > 2) {
        // Considero solo lo que esta dentro de los [ ]
        cadena[len-1] = '\0';
        char* cursor = cadena + 1;

        char* endptr_error;
        while (*cursor && !invalido) {
            avanzar_hasta_digito(&cursor);

            char* numero_str = cursor;
            avanzar_hasta_nodigito(&cursor);
            char temp = *cursor;
            *cursor = '\0';
            long int l_numero = strtol(numero_str, &endptr_error, 10);
            if (l_numero < INT_MAX) {
                // Verifico que no hubo overflow ya que las
                // funciones (copy, cmp...) de la lista operan con int's
                int numero = (int)l_numero;
                lista_insertar_natural_derecha(lista, numero);
            } else {
                invalido = 1;
            }

            // Devuelvo el valor anterior para no salir del while
            // (*cursor == '\0') y moverme hasta el siguiente numero.
            *cursor = temp;
        }
    }

    if (invalido) {
        destruir_lista(lista);
        return NULL;
    }

    return lista;
}

void lista_insertar_natural_derecha(Lista* lista, int natural) {
    int temp = natural;
    glist_insertar_final(lista, &temp);
}

void lista_insertar_natural_izquierda(Lista* lista, int natural) {
    int temp = natural;
    glist_insertar_inicio(lista, &temp);
}

void lista_aumentar_derecha(Lista* lista) {
    int* valor_aumentar = (int*)glist_ultimo(lista);
    if (valor_aumentar) {
        (*valor_aumentar)++;
    }
}

void lista_aumentar_izquierda(Lista* lista) {
    int* valor_aumentar = (int*)glist_primero(lista);
    if (valor_aumentar) {
        (*valor_aumentar)++;
    }
}

void lista_eliminar_derecha(Lista* lista) {
    // Como en mi utilizacion de la interfaz GList siempre voy
    // a llamar esta funcion con listas no vacias puedo obviar el retorno.
    int _retorno = glist_eliminar_final(lista);
}

void lista_eliminar_izquierda(Lista* lista) {
    // Idem lista_eliminar_derecha
    int _retorno = glist_eliminar_inicio(lista);
}

void destruir_lista(Lista* lista) {
    glist_destruir(lista);
}

Lista* copiar_lista(const Lista* lista) {
    return glist_copiar(lista);
}

void visitar_lista(const Lista* lista) {
    glist_imprimir(lista);
}

static Lista* lista_crear() {
    Lista* lista = glist_crear((FuncionComparadora)cmp_int,
                                (FuncionDestructora)destruir_int,
                                (FuncionVisitante)visitar_int,
                                (FuncionCopia)copiar_int);
    return lista;
}

int definir_lista(char* nombre, void* lista, Declaraciones declaraciones) {
    // Creo mi declaracion temporal, al salir de aca se destruye,
    // luego se destruye tambien la lista en el main.c
    Declaracion declaracion;
    declaracion.nombre = nombre;
    declaracion.valor = (Lista*)lista;
    declaracion.tipo = LISTA;

    return guardar_declaracion(declaraciones, &declaracion);
}




