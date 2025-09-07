#ifndef SEARCH_H
#define SEARCH_H
#include "funciones.h"
#include "pila.h"

/*
 * Array de char* conteniendo los nombres de las listas a aplicar search.
 * [L11, L12, L21, L22, ...]
 */
typedef struct {
    GArray* garray;
} SearchExpr;

/*
 * Estructura que representa un estado de una lista en el proceso del Search, queda completamente
 * determinado por el contenido de la lista y la profundidad dentro del arbol de composiciones.
 */
typedef struct {
    Lista* lista;
    unsigned int profundidad;
} EstadoLista;

typedef HashTable* MemoEstados;

/* --------- Funciones usadas en MemoEstados --------- */
/*
 * Implementacion clasica del popular algoritmo hash_combine de la biblioteca Boost de C++,
 * ampliamente testeado y util para combinar resultados de hasheos previos en uno final, en
 * mi caso mezclo el hash de la lista con la profundidad de la misma.
 */
unsigned long hash_estado(const EstadoLista* estado);
int cmp_estado(const EstadoLista* a, const EstadoLista* b);
EstadoLista* copiar_estado(const EstadoLista* estado);
void destruir_estado(EstadoLista* estado);
void visitar_estado(const EstadoLista* estado);


/*
 * Ya recibe la expresion verificada que es una expresion no vacia y que tiene nombres de listas dentro separados
 * como se espera con '; ' y ', '. Lee la string en dicho formato y por cada lista que se encuentra, decide si es
 * una lista existente en la tabla e inserta dicho identifacador en el GArray subyacente.
 * Modifica la cadena original, no hay problema, es el ultimo eslabón del parser.
 */
SearchExpr* strsearch_to_search(char* cursor, Declaraciones declaraciones);

/*
 * Crea una expresion de Search, con el GArray correspondiente.
 */
SearchExpr* searchexpr_crear();

/*
 * Destruye una expresion de search.
 */
void destruir_search_expr(SearchExpr* search);

/*
 * Agrega una lista a tener en cuenta para la expresion del search.
 */
void agregar_expr(SearchExpr* search, char* lista);

/*
 * Imprime las listas a aplicar la busqueda.
 */
void visitar_search(const SearchExpr* search);

/*
 * Retorna la cantidad de listas en la expresion search, siempre multiplo de 2.
 */
unsigned int cantidad_listas_search(const SearchExpr* search);

/*
 * Entrada publica a la operacion search, inicializa las estructuras de datos a utilizar en la busqueda,
 * separa las 2 primeras listas, llama al algoritmo de busqueda y maneja la salida y liberacion de memoria.
 */
Funcion* search(Declaraciones declaraciones, SearchExpr* search);

/*
 * Dada una pila de funciones, construye una funcion resultado de componer todas las de la pila.
 * Nota:
 * Realiza una copia auxiliar en un array temporal de las funciones, luego recorre ese array en orden inverso para
 * formar la funcion final en el orden que las funciones de listas son aplicadas segun la teoria.
 */
Funcion* reconstruir_funcion_backtracking(Pila* pila);

/*
 * Crea la tabla de EstadosLista usada para llevar constancia de en que estados he estado.
 */
MemoEstados crear_tabla_estados();

/*
 * Destruye la tabla de estados.
 */
void destruir_tabla_estados(MemoEstados estados);

/*
 * Devuelve 1 si el estado ya se encuentra en la tabla de estados, si no 0.
 */
int estado_ya_visitado(MemoEstados estados, EstadoLista* estado);

/*
 * Almacena el estado en la tabla de estados.
 */
void recordar_estado_visitado(MemoEstados estados, EstadoLista* estado_clave, EstadoLista* estado_valor);

#endif
