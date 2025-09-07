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

static Funcion* dfs_busqueda(Lista* estado_actual, Lista* objetivo,
                             Pila* pila, Declaraciones declaraciones,
                             FuncionesAll* funciones_todas, unsigned int cant_funciones_definidas,
                             SearchExpr* pares, HashTable* estados_visitados);
static int comprobar_composicion_candidata(Funcion* funcion, SearchExpr* pares, Declaraciones declaraciones);
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
    // Pongo todas las funciones (base + definidas por el usuario) auxiliarmente
    // en memoria continua en un GArray para recorrerlas mas eficientemente varias veces.
    FuncionesAll* funciones_todas = obtener_todas_funciones_declaradas(declaraciones);
    Pila* backtracking_funciones = pila_crear((FuncionComparadora)comparar_declaracion,
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
        // Unico caso donde hago una aplicacion de composiciones desde 0 compleja
        Funcion* compuesta_candidata = reconstruir_funcion_backtracking(pila);

        int valido = comprobar_composicion_candidata(compuesta_candidata, pares, declaraciones);
        if (valido)
            return compuesta_candidata;  // Encontré una solución

        destruir_funcion(compuesta_candidata);
    }

    /* Cortar si alcanzo profundidad máxima */
    if (pila_elementos(pila) >= MAX_PROFUNDIDAD)
        return NULL;

    /* Estado actual en el que me encuentro (lista + profundidad) */
    EstadoLista estado = { estado_actual, pila_elementos(pila) };

    Funcion* resultado = NULL;
    for (int i = 0; i < cant_funciones_definidas && resultado == NULL; i++) {
        if (!estado_ya_visitado(estados_visitados, &estado)) {
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
                        /* Elimino la funcion con la que ya probe todas las combinaciones hacia abajo en el arbol */
                        pila_pop(pila);
                }

                // Destruyo copia que generó apply, haya o no haya encontrado la solucion
                destruir_lista(estado_intermedio);
            }
        }
    }

    /* Una vez que ya probé con todas las combinaciones de composiciones a partir de
     * este [estado lista actual + pronfundiad], lo recuerdo para no
     * volver a intentarlas sin sentido si llego a volver a llegar a este estado */
    recordar_estado_visitado(estados_visitados, &estado, &estado);
    return resultado;
}

// Prueba a partir de la 3er lista, o sea i = 2, o sea 2do par.
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
            visitar_lista(resultadoli1.lista_resultado);
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

        for (int i = 0; !pila_vacia(_pila) && i < largo; i++) {
            Declaracion* d = pila_top(_pila);
            funciones_buen_orden[i] = str_dup(d->nombre);  // Guardo la funcion "compactada" en el caso que sea
                                                           // una custom, no expando su definicion en sus funciones base.
            pila_pop(_pila);
        }

        Funcion* funcion = funcion_crear();
        for (int i = (int)largo - 1; i >= 0 ; i--)
            componer_funcion(funcion, funciones_buen_orden[i]);

        for (int i = 0; i < largo; i++)
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
        100019,
        (FuncionHash)hash_estado,
        (FuncionComparadora)cmp_estado,
        (FuncionDestructora)destruir_estado,
        (FuncionDestructora)destruir_estado,
        (FuncionCopia)copiar_estado,
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

void recordar_estado_visitado(MemoEstados estados, EstadoLista* estado_clave, EstadoLista* estado_valor) {
    // No me es relevante el saber si el estado estaba repetido o no.
    int _repetido = tabla_hash_insertar(estados, estado_clave, estado_valor);
    (int)_repetido;
}

unsigned long hash_estado(const EstadoLista* estado) {
    unsigned long h = hash_lista(estado->lista);
    h ^= estado->profundidad + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
    return h;
}

int cmp_estado(const EstadoLista* a, const EstadoLista* b) {
    if (listas_iguales(a->lista, b->lista) && a->profundidad == b->profundidad)
        return 0;

    return a->profundidad < b->profundidad ? -1 : 1;
}

EstadoLista* copiar_estado(const EstadoLista* estado) {
    EstadoLista* nuevo = malloc(sizeof(EstadoLista));
    nuevo->lista = copiar_lista(estado->lista);
    nuevo->profundidad = estado->profundidad;
    return nuevo;
}

void destruir_estado(EstadoLista* estado) {
    destruir_lista(estado->lista);
    free(estado);
}

void visitar_estado(const EstadoLista* estado) {
    printf("Profundidad=%u, Lista=", estado->profundidad);
    visitar_lista(estado->lista);
}
