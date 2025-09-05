#include "search.h"
#include "string_utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGO_SEARCH 200

// search { L1, L2; L3, L4; }
SearchExpr* strsearch_to_search(char* cursor, Declaraciones declaraciones) {
    cursor += 7;
    avanzar_hasta_noespacio(&cursor); // Me paro en {
    cursor++;
    char* fin_search = strchr(cursor, '}'); // Ya sé que existe dicho }
    *fin_search = '\0';

    cursor = str_trim(cursor); // Me quedo solo con lo que arranque en la primera lista y termine en el ultimo '; '
    printf("%s\n", cursor);
    int cantidad = 0, listas_validas = 1;
    SearchExpr* search_expr = searchexpr_crear();

    while (*cursor && listas_validas) {
        avanzar_hasta_noespacio(&cursor);
        char* nombre_lista = cursor;
        avanzar_hasta_pcoma(&cursor);
        char temp = *cursor;
        *cursor = '\0';
        if (obtener_def_usuario(declaraciones, nombre_lista, LISTA))
            agregar_expr(search_expr, nombre_lista);
        else
            listas_validas = 0;

        *cursor = temp;
        cursor++; // Me muevo luego del ', ' o '; '
        cantidad++;
    }

    if (!listas_validas || !cantidad || cantidad % 2 != 0) {
        printf("%d %d \n", cantidad, listas_validas);
        destruir_search_expr(search_expr);
        return NULL;
    }
    return search_expr;
}

SearchExpr* searchexpr_crear() {
    SearchExpr* search = malloc(sizeof(SearchExpr));
    assert(search != NULL);
    search->garray = garray_crear(LARGO_SEARCH,
                                    (FuncionComparadora)cmp_str,
                                    (FuncionDestructora)destruir_str,
                                    (FuncionVisitante)visitar_str,
                                    (FuncionCopia)copiar_str);
    return search;
}


void destruir_search_expr(SearchExpr* search) {
    garray_destruir(search->garray);
    free(search);
}

void agregar_expr(SearchExpr* search, char* lista) {
    garray_insertar(search->garray, lista);
}

void visitar_search(const SearchExpr* search) {
    garray_imprimir(search->garray);
}
