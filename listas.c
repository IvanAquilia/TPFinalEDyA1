#include "listas.h"
#include "string_utils.h"
#include "utils.h"
#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

            // Uso strtoul en vez de strol ya que dependiendo
            // del modelo de programacion se tiene que:
            // unsigned int (unsigned int) > long int | (en LP64, UNIX)
            // long int > unsigned int (unsigned int) | (en LLP64, Windows)
            // sin embargo usando unsigned long me aseguro siempre que va a entrar para luego chequear overflow.
            unsigned long l_numero = strtoul(numero_str, &endptr_error, 10);
            if (l_numero < UINT_MAX) {
                unsigned int numero = (unsigned int)l_numero;
                lista_insertar_numero_derecha(lista, numero);
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

Lista* lista_crear() {
    Lista* lista = malloc(sizeof(Lista));
    assert(lista != NULL);
    char* tipo = malloc(sizeof(char) * 5);
    assert(tipo != NULL);
    strcpy(tipo, "uint");
    lista->glist = glist_crear((FuncionComparadora)cmp_uint,
                                (FuncionDestructora)destruir_uint,
                                (FuncionVisitante)visitar_uint,
                                (FuncionCopia)copiar_uint,
                                tipo);
    free(tipo);
    return lista;
}

void lista_insertar_numero_derecha(Lista* lista, unsigned int natural) {
    unsigned int temp = natural;
    glist_insertar_final(lista->glist, &temp);
}

void lista_insertar_numero_izquierda(Lista* lista, unsigned int natural) {
    unsigned int temp = natural;
    glist_insertar_inicio(lista->glist, &temp);
}

void lista_aumentar_derecha(Lista* lista) {
    unsigned int* valor_aumentar = (unsigned int*)glist_ultimo(lista->glist);
    if (valor_aumentar) {
        (*valor_aumentar)++;
    }
}

void lista_aumentar_izquierda(Lista* lista) {
    unsigned int* valor_aumentar = (unsigned int*)glist_primero(lista->glist);
    if (valor_aumentar) {
        (*valor_aumentar)++;
    }
}

void lista_eliminar_derecha(Lista* lista) {
    int _aux = glist_eliminar_final(lista->glist);
    (int)_aux;
}

void lista_eliminar_izquierda(Lista* lista) {
    int _aux = glist_eliminar_inicio(lista->glist);
    (int)_aux;
}

void destruir_lista(Lista* lista) {
    glist_destruir(lista->glist);
    free(lista);
}

int lista_vacia(Lista* lista) {
    if (lista->glist->head == NULL)
        return 1;
    return 0;
}

unsigned int lista_longitud(const Lista* lista) {
    return lista->glist->longitud;
}

unsigned int* primer_elemento(Lista* lista) {
    unsigned int* primero = (unsigned int*)glist_primero(lista->glist);
    return primero;
}

unsigned int* ultimo_elemento(Lista* lista) {
    unsigned int* ultimo = (unsigned int*)glist_ultimo(lista->glist);
    return ultimo;
}

Lista* copiar_lista(const Lista* lista) {
    Lista* copia = malloc(sizeof(Lista));
    assert(copia != NULL);
    copia->glist = glist_copiar(lista->glist);
    return copia;
}

int listas_iguales(Lista* lista1, Lista* lista2) {
    return glist_iguales(lista1->glist, lista2->glist);
}

void visitar_lista(const Lista* lista) {
    printf("[");
    glist_imprimir(lista->glist);
    printf("]\n");
}

unsigned long hash_lista(const Lista* lista) {
    unsigned long h = 1469598103934665603ull;
    GList* glist_subyacente = lista->glist;
    Nodo* actual = glist_subyacente->head;
    while (actual) {
        unsigned int* val = actual->dato;
        h ^= *val;
        h *= 1099511628211ull;
        actual = actual->sig;
    }
    h ^= lista_longitud((Lista*)lista);
    return h;
}


