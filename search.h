#ifndef SEARCH_H
#define SEARCH_H
#include "funciones.h"
#include "pila.h"

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

unsigned int cantidad_pares_search(const SearchExpr* search);

Funcion* search(Declaraciones declaraciones, SearchExpr* search);

/*
 * Dada una pila de funciones, construye una funcion resultado de componer todas las de la pila.
 * Por la naturaleza inversa de como se aplican las funciones en funciones de listas a comparacion
 * de funciones matematicas clasicas, al componer las funciones en el orden que me las devuelve la pila,
 * es decir, desde la ultima que se aplicó hasta la primera, obtengo como resultado una funcion con orden
 * completamente inverso al calculado. En vez de tener que insertar al inicio del array desplazando
 * los elementos (costoso), para solucinar esto, reconstruyo de todos modos asi a sabiendas de
 * esto, y luego simplemente la imprimo por pantalla en orden inverso, obteniendo la funcion buscada.
 */
Funcion* reconstruir_funcion_backtracking(Pila* pila);
#endif
