#include "search.h"
#include "string_utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGO_SEARCH 200

/* 
 * En el caso ideal de solo tener las 6 funciones bases definidas y ninguna mas, el tiempo de recorrido
 * de MÁXIMO (es decir, el recorrido de todas las composiciones del arbol, excluyendo obviamente las podadas) con 
 * profundidad 10 es menor a 4-5 segundos, con profundidad 11 puede explotar hasta los 30seg.
 * Por otro lado en entornos controlados con las 6 funciones base + 3 definidas custom por el usuario
 * (con repeticiones, algo costosas) se verificó que hasta con un valor de profundidad de 10 se posee
 * un tiempo de recorrido MÁXIMO razonable menor a 25-30seg, a partir de 11 explota en crecimiento y puede
 * superar facilmente el minuto/minuto y medio de tiempo de recorrido máximo. De todos modos en la práctica
 * es extraño que se tenga que recorrer casi todo el arbol para encontrar una solucion, por lo que el tiempo
 * promedio es menor, esto son los casos extremos. Si se busca practicamente que sea instantáneo (dependiendo
 * de la cantidad de funciones definidas) se recomienda un valor entre 8 y 9.
 */
#define MAX_PROFUNDIDAD 10  

/*
 * Realiza la busqueda utilizando un algoritmo DFS-like, junto con una tabla memo en la que se guardan estados de lista,
 * que en mi caso son la lista, la profundidad y una firma para preguntar rapido si son el mismo estado.
 * AL finalizar la busqueda recursiva retorna la Funcion si la encontró, o si no NULL.
 */
static Funcion* dfs_busqueda(Lista* estado_actual, Lista* objetivo,
                             Pila* pila, Declaraciones declaraciones,
                             FuncionesAll* funciones_todas, unsigned int cant_funciones_definidas,
                             SearchExpr* pares, HashTable* estados_visitados);

/*
 * Dada una Funcion, comprueba si aplicar esa funcion para todas las Li1 las llevan a Li2, con i > 2.
 */
static int comprobar_composicion_candidata(Funcion* funcion, SearchExpr* pares, Declaraciones declaraciones);

/*
 * Revisa si, dada una funcion base, tiene sentido aplicarla al estado actual de la lista, revisando en la pila
 * de aplicaciones que fue lo ultimo que se aplicó, o si la lista es vacia.
 */
static int podar_casos_triviales(Funcion* funcion, Pila* pila, Lista* estado_actual);


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

unsigned int cantidad_listas_search(const SearchExpr* search) {
    return search->garray->tamaño_actual;
}

char* obtener_lista_iesima(const SearchExpr* search, unsigned int i) {
    return (char*)search->garray->elementos[i];
}

Funcion* search(Declaraciones declaraciones, SearchExpr* pares) {
    /* Pongo todas las funciones (base + definidas por el usuario) auxiliarmente
     * en memoria continua en un GArray para recorrerlas mas eficientemente varias veces. */
    FuncionesAll* funciones_todas = obtener_todas_funciones_declaradas(declaraciones);
    Pila* backtracking_funciones = pila_crear(
        (FuncionComparadora)comparar_declaracion,
        (FuncionDestructora)destruir_declaracion,
        (FuncionVisitante)visitar_declaracion,
        (FuncionCopia)copiar_declaracion,
        "declaraciones");
    MemoEstados estados_visitados = crear_tabla_estados();
    unsigned int cant_funciones_definidas = cantidad_funciones_totales(funciones_todas);

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
                                     pares, estados_visitados);

    pila_destruir(backtracking_funciones);
    destruir_arreglo_aux_funciones(funciones_todas);
    destruir_tabla_estados(estados_visitados);
    return resultado;
}

static Funcion* dfs_busqueda(Lista* estado_actual, Lista* objetivo, Pila* pila, Declaraciones declaraciones,
    FuncionesAll* funciones_todas, unsigned int cant_funciones_definidas,
    SearchExpr* pares, MemoEstados estados_visitados) {

    /* Caso base, me pauso a chequear si es valida la solucion */
    if (listas_iguales(estado_actual, objetivo)) {
        Funcion* compuesta_candidata = reconstruir_funcion_backtracking(pila);

        /* Unico caso donde realizo una aplicacion potencialmente costosa */
        int valido = comprobar_composicion_candidata(compuesta_candidata, pares, declaraciones);
        if (valido)
            return compuesta_candidata;
        destruir_funcion(compuesta_candidata);
    }

    /* Cortar si alcanzo profundidad máxima o si ya pase por esta lista + esta profundidad */
    EstadoLista estado = { estado_actual, pila_elementos(pila) };
    if (pila_elementos(pila) >= MAX_PROFUNDIDAD || estado_ya_visitado(estados_visitados, &estado))
        return NULL;

    Funcion* resultado = NULL;
    for (unsigned int i = 0; i < cant_funciones_definidas && resultado == NULL; i++) {
        Declaracion* declaracion = funcion_definida_iesima(funciones_todas, i);
        int factible_aplicar = podar_casos_triviales(declaracion->valor, pila, estado_actual);

        if (factible_aplicar) {
            ResultadoApply resultado_apply = aplicar_funcion(declaracion->valor, estado_actual, declaraciones);

            /* El "estado intermedio" aca se vuelve un "estado actual" dentro de la llamada recursiva siguiente */
            Lista* estado_intermedio = resultado_apply.lista_resultado;
            if (resultado_apply.status == 0) {
                pila_push(pila, declaracion);
                resultado = dfs_busqueda(estado_intermedio,
                    objetivo, pila, declaraciones,
                    funciones_todas, cant_funciones_definidas,
                    pares, estados_visitados);
                if (resultado == NULL)
                    pila_pop(pila);
            }

            /* Destruyo copia que generó apply, haya o no haya encontrado la solucion */
            destruir_lista(estado_intermedio);
        }
    }

    /* Una vez que ya probé con todas las combinaciones de composiciones a partir de
     * este [estado lista actual + pronfundiad], lo recuerdo para no
     * volver a intentarlas sin sentido si llego a volver a llegar a este estado */
    recordar_estado_visitado(estados_visitados, &estado);
    return resultado;
}

static int comprobar_composicion_candidata(Funcion* funcion, SearchExpr* pares, Declaraciones declaraciones) {
    if (funcion == NULL)
        return 0;

    unsigned int cantidad_pares = cantidad_listas_search(pares);
    int candidata_valida = 1;
    if (cantidad_pares > 2) {
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
    }
    return candidata_valida;
}

Funcion* reconstruir_funcion_backtracking(Pila* pila) {
    if (strcmp(pila->glist->tipo, "declaraciones") == 0) {
        Pila* _pila = copiar_pila(pila); // Copio para no destruir el backtracking generado hasta ahora en el caso que
                                         // esta funcion no aplique para los demas pares.

        unsigned int largo = pila_elementos(_pila);
        char* funciones_buen_orden[largo];

        for (unsigned int i = 0; !pila_vacia(_pila) && i < largo; i++) {
            Declaracion* d = pila_top(_pila);
            funciones_buen_orden[i] = str_dup(d->nombre);  // Guardo la funcion "compactada" en el caso que sea
                                                           // una custom, no expando su definicion en sus funciones base.
            pila_pop(_pila);
        }

        Funcion* funcion = funcion_crear();
        for (int i = (int)largo - 1; i >= 0 ; i--)
            componer_funcion(funcion, funciones_buen_orden[i]);

        for (unsigned int i = 0; i < largo; i++)
            free(funciones_buen_orden[i]);

        pila_destruir(_pila);
        return funcion;
    }

    return NULL;
}

static int podar_casos_triviales(Funcion* funcion, Pila* pila, Lista* estado_actual) {

    int factible_aplicar = 1;
    Declaracion* _d = pila_top(pila);
    Funcion* ultima_funcion_aplicada = _d != NULL ? _d->valor : NULL;
    if (ultima_funcion_aplicada != NULL && cantidad_composiciones(ultima_funcion_aplicada) == 1 && cantidad_composiciones(funcion) == 1) {
        TipoFuncion tipo_a_aplicar = str_a_tipo(funcion_iesima(funcion, 0));
        TipoFuncion tipo_aplicada = str_a_tipo(funcion_iesima(ultima_funcion_aplicada, 0));

        if (tipo_a_aplicar != Custom && tipo_aplicada != Custom) {
            unsigned int longitud = lista_longitud(estado_actual);
            if (longitud > 1) {
                /* Agregar y sacar un 0 de algun extremo */
                if ((tipo_aplicada == Od && tipo_a_aplicar == Dd) || (tipo_aplicada == Oi && tipo_a_aplicar == Di)) {
                    factible_aplicar = 0;
                }
            } else if (longitud == 1) {
                /* Agregar y sacar un 0 de una funcion de un elemento */
                if ((tipo_aplicada == Od || tipo_aplicada == Oi) && (tipo_a_aplicar == Dd || tipo_a_aplicar == Di)) {
                    factible_aplicar = 0;
                }
            } else {
                /* Evitar llamar a apply si ya sé que va a ser inválida la operacion */
                if (tipo_a_aplicar == Si || tipo_a_aplicar == Sd || tipo_a_aplicar == Di || tipo_a_aplicar == Dd) {
                    factible_aplicar = 0;
                }
            }
        }
    }

    return factible_aplicar;
}

MemoEstados crear_tabla_estados() {
    MemoEstados estados = tabla_hash_crear(
        300007,
        (FuncionHash)hash_estado,
        (FuncionComparadora)cmp_estado,
        (FuncionDestructora)destruir_uint,
        (FuncionDestructora)destruir_estado,
        (FuncionCopia)copiar_uint,
        (FuncionCopia)copiar_estado,
        (FuncionVisitante)visitar_estado
    );

    return estados;
}

void destruir_tabla_estados(MemoEstados estados) {
    tabla_hash_destruir(estados);
}

int estado_ya_visitado(MemoEstados estados, EstadoLista* estado) {
    EstadoLista* encontrado = tabla_hash_buscar(estados, estado);
    if (encontrado == NULL)
        return 0;

    return 1;
}

void recordar_estado_visitado(MemoEstados estados, EstadoLista* estado_clave) {
    // No me es relevante el saber si el estado estaba repetido o no.
    unsigned int _dummy = 1;
    int _repetido = tabla_hash_insertar(estados, estado_clave, &_dummy);
    (void)_repetido;
}

