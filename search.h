#ifndef SEARCH_H
#define SEARCH_H
#include "garray.h"
#include "listas.h"

/*
 * Array de char*, similiar al tipo Funcion*
 * [L11, L12, L21, L22, ...]
 */
typedef struct {
    GArray* garray;
} SearchExpr;

SearchExpr* strsearch_to_search(char* cursor, Declaraciones declaraciones);

SearchExpr* searchexpr_crear();

void destruir_search_expr(SearchExpr* search);

void agregar_expr(SearchExpr* search, char* lista);

void visitar_search(const SearchExpr* search);
#endif
