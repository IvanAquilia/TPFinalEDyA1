#include "search.h"
#include "string_utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funciones.h"
#include "pila.h"

#define LARGO_SEARCH 200
#define MAX_PROFUNDIDAD 10

static Funcion* dfs_busqueda(Lista* actual, Lista* objetivo,
                        Pila* pila, Declaraciones declaraciones,
                        FuncionesAll* funciones_todas, unsigned int cant_funciones_definidas,
                        SearchExpr* pares);
static int comprobar_composicion_candidata(Funcion* funcion, SearchExpr* pares, Declaraciones declaraciones);

SearchExpr* strsearch_to_search(char* cursor, Declaraciones declaraciones) {
    cursor += 7;
    avanzar_hasta_noespacio(&cursor); // Me paro en {
    cursor++;
    char* fin_search = strchr(cursor, '}'); // Ya sé que existe dicho }
    *fin_search = '\0';

    cursor = str_trim(cursor); // Me quedo solo con lo que este entre la primera lista y el ultimo '; '
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

unsigned int cantidad_pares_search(const SearchExpr* search) {
    return search->garray->tamaño_actual;
}

char* obtener_lista_iesima(const SearchExpr* search, unsigned int i) {
    return (char*)search->garray->elementos[i];
}

Funcion* search(Declaraciones declaraciones, SearchExpr* pares) {
    // Pongo todas las funciones (base + definidas por el usuario) auxiliarment
    // en memoria continua en un GArray para recorrerlas mas eficientemente varias veces.
    FuncionesAll* funciones_todas = obtener_todas_funciones_declaradas(declaraciones);
    unsigned int cant_funciones_definidas = cantidad_funciones_totales(funciones_todas);
    Pila* backtracking_funciones = pila_crear((FuncionComparadora)comparar_funcion,
                                                (FuncionDestructora)destruir_funcion,
                                                (FuncionVisitante)visitar_funcion,
                                                (FuncionCopia)copiar_funcion,
                                                "funcion");

    char* nombre_lista_inicio = obtener_lista_iesima(pares, 0);
    char* nombre_lista_objetivo = obtener_lista_iesima(pares, 1);
    Lista* inicio = obtener_def_usuario(declaraciones, nombre_lista_inicio, LISTA);
    Lista* objetivo = obtener_def_usuario(declaraciones, nombre_lista_objetivo, LISTA);

    // Uso como pivot de de busqueda al primer par, en el momento que encuentre
    // una funcion que verifique la condicion con este, pruebo si lo hace con los demás pares,
    // pero siempre busco sobre este par.
    Funcion* resultado = dfs_busqueda(inicio, objetivo,
                                     backtracking_funciones, declaraciones,
                                     funciones_todas, cant_funciones_definidas,
                                     pares);

    pila_destruir(backtracking_funciones);
    destruir_arreglo_aux_funciones(funciones_todas);
    return resultado;
}

static Funcion* dfs_busqueda(Lista* actual, Lista* objetivo,
                        Pila* pila, Declaraciones declaraciones,
                        FuncionesAll* funciones_todas, unsigned int cant_funciones_definidas,
                        SearchExpr* pares) {

    Funcion* resultado = NULL;
    // Caso base: lista actual == objetivo
    if (listas_iguales(actual, objetivo)) {

        // Unico caso donde hago una aplicacion compleja
        Funcion* compuesta_candidata = reconstruir_funcion_backtracking(pila);
        int valido = comprobar_composicion_candidata(compuesta_candidata, pares, declaraciones);

        if (valido)
            resultado = compuesta_candidata;
            return resultado;  // Encontré una solución
    }

    // Cortar si alcanzamos profundidad máxima
    if (pila_elementos(pila) >= MAX_PROFUNDIDAD)
        return NULL;

    // Probar todas las funciones
    for (int i = 0; i < cant_funciones_definidas && resultado == NULL; i++) {
        Funcion* funcion = funcion_definida_iesima(funciones_todas, i);

        ResultadoApply resultado_apply = aplicar_funcion(funcion, actual, declaraciones);
        //
        // CHEQUEAR ACA SI HUBO OVERFLOW ERRO Y ESO Y SINO NO COMPONER O SEA CHEQUEAR RESULTADO.STATUS
        //
        Lista* lista_resultado = resultado_apply.lista_resultado;

        // Apilo y sigo recursivamente
        pila_push(pila, funcion);
        resultado = dfs_busqueda(lista_resultado, objetivo, pila,
                                  declaraciones,
                                  funciones_todas, cant_funciones_definidas,
                                  pares);

        destruir_lista(lista_resultado); // Destruyo copia que genero apply haya o no haya encontrado la solucion

        if (!resultado)
            pila_pop(pila); // Elimino la funcion con la que probe todas las combinaciones
                            // hacia abajo en el arbol tomo la siguiente y repito.
    }

    return resultado;
}

// Prueba a partir de la 3er lista, o sea i = 2, o sea 2do par.
static int comprobar_composicion_candidata(Funcion* funcion, SearchExpr* pares, Declaraciones declaraciones) {
    unsigned int cantidad_pares = cantidad_pares_search(pares);
    int candidata_valida = 1;
    for (unsigned int i = 2; i < cantidad_pares && candidata_valida; i += 2) {
        char* li1_nombre = obtener_lista_iesima(pares, i);
        char* li2_nombre = obtener_lista_iesima(pares, i + 1);

        Lista* li1 = obtener_def_usuario(declaraciones, li1_nombre, LISTA);
        Lista* li2 = obtener_def_usuario(declaraciones, li2_nombre, LISTA);

        ResultadoApply resultadoli1 = aplicar_funcion(funcion, li1, declaraciones);
        if (resultadoli1.status != 0 || !listas_iguales(resultadoli1.lista_resultado, li2))
            candidata_valida = 0;

        destruir_lista(resultadoli1.lista_resultado);
    }

    return candidata_valida;
}

Funcion* reconstruir_funcion_backtracking(Pila* pila) {
    if (strcmp(pila->glist->tipo, "funcion") == 0) {
        Funcion* funcion = funcion_crear();
        while (!pila_vacia(pila)) {
            Funcion* f = pila_top(pila);
            for (unsigned int i = 0; i < cantidad_composiciones(f); i++) {
                char* f_i = funcion_iesima(f, i);
                componer_funcion(funcion, f_i);
            }
            pila_pop(pila);
        }
        return funcion;
    }

    return NULL;
}
