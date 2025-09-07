#include "funciones.h"
#include "string_utils.h"
#include "utils.h"
#include "listas.h"
#include "parser.h"
#include "pila.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGO_FUNCIONES 200
/* 2^19 aplicaciones de funciones (una base, comenzar/terminar repeticiones, aplicar una custom) por apply */
#define MAX_OPERACIONES 524288
/* 10.000 llamadas recursivas maximo por apply. */
#define MAX_MEMORIA 10000

/*
 * Realiza la logica interna real de aplicar una función, devuelve 0 si pudo concretar el aplicado, 1 si hubo alguna
 * operacion ilegal en el proceso (eliminación o sucesor a []), o -1 si excedió las constantes de max_operaciones o
 * max_memoria.
 */
static int aplicar_funcion_interno(Funcion* funcion, Lista* lista, int* acc_operaciones,
                                    int* acc_memoria, Declaraciones declaraciones);
/*
 * Dado una unica función, decide mediante su nombre si se trata de una función, si es así la aplica.
 * Si no, busca dicha función custom y repite el proceso llamando a aplicar_funcion_interno().
 */
static int aplicar_base_o_rebuscar(char* nombre_funcion, Lista* lista, int* acc_operaciones,
                                    int* acc_memoria, Declaraciones declaraciones);
/*
 * Recibe la posición donde se encuentra el signo de abrir repetición ('<'),
 * Devuelve la posición del '>' que cierra dicha repetición.
 */
static unsigned int saltear_repeticion(Funcion* funcion, unsigned int i);


Funcion* strfunc_to_funcion(char* cadena, Declaraciones declaraciones) {
    Funcion* funcion = funcion_crear();
    char* cursor = cadena;
    int funcion_inexistente = 0;

    while (*cursor && !funcion_inexistente) {
        avanzar_hasta_noespacio(&cursor);

        if (*cursor == '<' || *cursor == '>') {
            /* Tomo a los simbolos de inicio y fin de repeticion como unos "operadores" o "funciones" más.
             * Estructuracion muy util luego a la hora de aplicar la funcion. */
            char simbolo_repeticion[2] = { *cursor, '\0' };

            componer_funcion(funcion, simbolo_repeticion);
            cursor++;
        } else if (*cursor) {
            char* nombre = cursor;
            avanzar_hasta_espacio_o_repeticion(&cursor);
            char temp = *cursor;
            *cursor = '\0';

            /* Chequeo que la funcion a componer exista. */
            if (obtener_def_usuario(declaraciones, nombre, FUNCION))
                componer_funcion(funcion, nombre);
            else
                funcion_inexistente = 1;

            /* Devuelvo el valor previo a poner el '\0' para tokenizar, de este modo
             * no salgo del while (*cursor == '\0') y me muevo hasta la siguiente funciónn. */
            *cursor = temp;
        }
    }

    if (funcion_inexistente) {
        destruir_funcion(funcion);
        return NULL;
    }
    return funcion;
}

Funcion* funcion_crear() {
    Funcion* funcion = malloc(sizeof(Funcion));
    assert(funcion != NULL);
    funcion->garray = garray_crear(
        LARGO_FUNCIONES,
        (FuncionComparadora)cmp_str,
        (FuncionDestructora)destruir_str,
        (FuncionVisitante)visitar_str,
        (FuncionCopia)copiar_str);

    return funcion;
}

FuncionesAll* obtener_todas_funciones_declaradas(Declaraciones declaraciones) {
    FuncionesAll* funciones_all = malloc(sizeof(FuncionesAll));
    assert(funciones_all != NULL);
    funciones_all->garray = garray_crear(
        declaraciones->elementos,
        (FuncionComparadora)comparar_declaracion,
        (FuncionDestructora)destruir_declaracion,
        (FuncionVisitante)visitar_declaracion,
        (FuncionCopia)copiar_declaracion);

    for (unsigned int i = 0; i < declaraciones->capacidad; i++) {
        if (declaraciones->buckets[i] != NULL) {
            Declaracion* declaracion = declaraciones->buckets[i]->dato;
            if (declaracion->tipo == FUNCION) {
                garray_insertar(funciones_all->garray, declaracion);
            }
        }
    }

    return funciones_all;
}

void componer_funcion(Funcion* funcion, char* nombre) {
    garray_insertar(funcion->garray, nombre);
}

void destruir_funcion(Funcion* funcion) {
    garray_destruir(funcion->garray);
    free(funcion);
}

void destruir_arreglo_aux_funciones(FuncionesAll* funciones_todas) {
    garray_destruir(funciones_todas->garray);
    free(funciones_todas);
}

Funcion* copiar_funcion(const Funcion* funcion) {
    Funcion* copia = malloc(sizeof(Funcion));
    assert(copia != NULL);
    copia->garray = garray_copiar(funcion->garray);
    return copia;
}

void visitar_funcion(const Funcion* funcion) {
    garray_imprimir(funcion->garray);
}

int funciones_iguales(const Funcion* funcion1, const Funcion* funcion2) {
    int iguales = 1;
    if (cantidad_composiciones(funcion1) == cantidad_composiciones(funcion2)) {
        for (unsigned int i = 0; i < cantidad_composiciones(funcion1) && iguales; i++) {
            char* funcion_i_1 = funcion_iesima(funcion1, i);
            char* funcion_i_2 = funcion_iesima(funcion2, i);
            if (strcmp(funcion_i_1, funcion_i_2) != 0) {
                iguales = 0;
            }
        }
    } else {
        iguales = 0;
    }

    return iguales;
}

TipoFuncion str_a_tipo(char* nombre_funcion) {
    TipoFuncion tipo = Custom;
    if (strcmp(nombre_funcion, "Oi") == 0)
        tipo = Oi;
    if (strcmp(nombre_funcion, "Od") == 0)
        tipo = Od;
    if (strcmp(nombre_funcion, "Si") == 0)
        tipo = Si;
    if (strcmp(nombre_funcion, "Sd") == 0)
        tipo = Sd;
    if (strcmp(nombre_funcion, "Di") == 0)
        tipo = Di;
    if (strcmp(nombre_funcion, "Dd") == 0)
        tipo = Dd;

    return tipo;
}

unsigned int cantidad_composiciones(const Funcion* funcion) {
    return funcion->garray->tamaño_actual;
}

unsigned int cantidad_funciones_totales(const FuncionesAll* funciones) {
    return funciones->garray->tamaño_actual;
}

char* funcion_iesima(const Funcion* funcion, unsigned int i) {
    return (char*)garray_obtener(funcion->garray, i);
}

Declaracion* funcion_definida_iesima(const FuncionesAll* funciones, unsigned int i) {
    return (Declaracion*)garray_obtener(funciones->garray, i);
}

int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
    char* nombre_funcion, char* string_lista,
    int in_place, Declaraciones declaraciones) {

    *funcion = (Funcion*)obtener_def_usuario(declaraciones, nombre_funcion, FUNCION);

    if (in_place && verificar_lista(string_lista))
        *lista = strlist_to_lista(string_lista);
    else if (!in_place)
        *lista = (Lista*)obtener_def_usuario(declaraciones, string_lista, LISTA);

    if (*funcion && *lista)
        return 1;
    return 0;
}

ResultadoApply aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones) {
    Lista* list_temp = copiar_lista(lista);
    int* acc_operaciones = malloc(sizeof(int));
    int* acc_memoria = malloc(sizeof(int));
    *acc_operaciones = 0;
    *acc_memoria = 0;
    int status = aplicar_funcion_interno(funcion, list_temp, acc_operaciones, acc_memoria, declaraciones);

    ResultadoApply resultado;
    resultado.status = status;
    resultado.lista_resultado = list_temp;

    free(acc_operaciones);
    free(acc_memoria);
    return resultado;
}

static int aplicar_funcion_interno(Funcion* funcion, Lista* lista, int* acc_operaciones,
    int* acc_memoria, Declaraciones declaraciones) {

    Pila* pila_repeticiones = pila_crear(
        (FuncionComparadora)cmp_uint,
        (FuncionDestructora)destruir_uint,
        (FuncionVisitante)visitar_uint,
        (FuncionCopia)copiar_uint,
        "uint");

    unsigned int cantidad = cantidad_composiciones(funcion), i = 0;
    int status = 0;

    while (i < cantidad && status == 0 && *acc_operaciones < MAX_OPERACIONES && *acc_memoria < MAX_MEMORIA) {
        char* f = funcion_iesima(funcion, i);
        if (strcmp(f, "<") == 0)
            /* Chequeo si es factible comenzar la repetición, sino la salteo.
             * primer_elemento/ultimo_elemento nunca devolverian NULL, debido a
             * la condicion previa de lista_longitud, es seguro des-referenciar. */
            if (lista_longitud(lista) && *primer_elemento(lista) != *ultimo_elemento(lista))
                pila_push(pila_repeticiones, &i);
            else
                i = saltear_repeticion(funcion, i);
        else if (strcmp(f, ">") == 0)
            /* Vuelvo a chequear si es factible, si no lo es, hago pop y sigo de largo, asi no tengo
             * el costo lineal de saltear_repeticion() hasta este '>' en el caso que ya no haga falta repetir. */
            if (lista_longitud(lista) && *primer_elemento(lista) != *ultimo_elemento(lista)) {
                unsigned int* pos_apertura_rep = pila_top(pila_repeticiones);
                i = *pos_apertura_rep;
            }
            else
                pila_pop(pila_repeticiones);
        else
            status = aplicar_base_o_rebuscar(f, lista, acc_operaciones, acc_memoria, declaraciones);

        i++;
        *acc_operaciones += 1;
    }

    pila_destruir(pila_repeticiones);
    if (*acc_operaciones == MAX_OPERACIONES || *acc_memoria == MAX_MEMORIA)
        status = -1;
    return status;
}

static int aplicar_base_o_rebuscar(char* nombre_funcion, Lista* lista, int* acc_operaciones,
    int* acc_memoria, Declaraciones declaraciones) {

    TipoFuncion tipo = str_a_tipo(nombre_funcion);
    int valido = 0;

    switch (tipo) {
        case Oi:
            lista_insertar_numero_izquierda(lista, 0);
            break;
        case Od:
            lista_insertar_numero_derecha(lista, 0);
            break;
        case Si:
            if (!lista_vacia(lista))
                lista_aumentar_izquierda(lista);
            else
                valido = 1;
            break;
        case Sd:
            if (!lista_vacia(lista))
                lista_aumentar_derecha(lista);
            else
                valido = 1;
            break;
        case Di:
            if (!lista_vacia(lista))
                lista_eliminar_izquierda(lista);
            else
                valido = 1;
            break;
        case Dd:
            if (!lista_vacia(lista))
                lista_eliminar_derecha(lista);
            else
                valido = 1;
            break;
        case Custom:
            /* Busco la funcion custom y re-aplico el mismo proceso recursivamente */
            Funcion* funcion = obtener_def_usuario(declaraciones, nombre_funcion, FUNCION);
            *acc_memoria += 1;
            return aplicar_funcion_interno(funcion, lista, acc_operaciones, acc_memoria, declaraciones);
    }

    return valido;
}

static unsigned int saltear_repeticion(Funcion* funcion, unsigned int i) {
    int nivel = 1; // Arranco en 1, teniendo en cuenta al '<' por el que acabo de entrar a esta funcion
    while (nivel != 0) {
        i++;
        char* f = funcion_iesima(funcion, i);
        if (strcmp(f, "<") == 0)
            nivel++;
        else if (strcmp(f, ">") == 0)
            nivel--;
    }

    return i;
}
