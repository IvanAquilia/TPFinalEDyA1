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
#define MAX_OPERACIONES 33554432 // 2^25


/*
 * Chequeo tambien al cerrar repeticion para no tener que volver en O(n)
 * hasta el '>' en el caso de no tener que repetir mas, es un gran ahorro de tiempo
 * en frente del costo marginal de tener que preguntar 2 veces lo mismo innecesariamente
 */
static int aplicar_funcion_interno(Funcion* funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones);
static int aplicar_base_o_rebuscar(char* nombre_funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones);
static unsigned int saltear_repeticion(Funcion* funcion, unsigned int i);
static TipoFuncion str_a_tipo(char* nombre_funcion);

/*
 * Ya recibe string verificada que es una seguidilla de alpha +
 * espacios y con los < > chequeados, no hay problema
 * en modificar la original, se va a destruie luego de
 * todos modos, necesito pasarlo a lista/array
 */
Funcion* strfunc_to_array(char* cadena, Declaraciones declaraciones) {
    Funcion* funcion = funcion_crear();
    char* cursor = cadena;
    int funcion_inexistente = 0;
    while (*cursor && !funcion_inexistente) {
        avanzar_hasta_noespacio(&cursor);

        if (*cursor == '<' || *cursor == '>') {
            // Tomo a los simbolos de inicio y fin de repetcion como
            // unos "operadores" o "funciones" más. Estructuracion muy util luego a la hora
            // de aplicar la funcion.
            char simbolo_repeticion[2] = { *cursor, '\0' };

            componer_funcion(funcion, simbolo_repeticion);
            cursor++;
        } else if (*cursor) {
            char* nombre = cursor;
            avanzar_hasta_espacio_o_repeticion(&cursor);
            char temp = *cursor;
            *cursor = '\0';

            // Chequeo que la funcion a componer exista
            if (obtener_def_usuario(declaraciones, nombre, FUNCION))
                componer_funcion(funcion, nombre);
            else
                funcion_inexistente = 1;

            // Devuelvo el valor previo a poner '\0' para no salir
            // del while (*cursor == '\0') y moverme hasta la siguiente funcion.
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
    funcion->garray = garray_crear(LARGO_FUNCIONES,
                                    (FuncionComparadora)cmp_str,
                                    (FuncionDestructora)destruir_str,
                                    (FuncionVisitante)visitar_str,
                                    (FuncionCopia)copiar_str);
    return funcion;
}


void componer_funcion(Funcion* funcion, char* nombre) {
    garray_insertar(funcion->garray, nombre);
}

void destruir_funcion(Funcion* funcion) {
    garray_destruir(funcion->garray);
    free(funcion);
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

unsigned int cantidad_composiciones(const Funcion* funcion) {
    return funcion->garray->tamaño_actual;
}

char* funcion_iesima(Funcion* funcion, unsigned int i) {
    return (char*)funcion->garray->elementos[i];
}

int definir_funcion(char* nombre, void* funcion, Declaraciones declaraciones) {
    // Creo mi declaracion temporal, al salir de aca se destruye,
    // luego se destruye tambien la funcion en el main.c
    Declaracion declaracion;
    declaracion.nombre = nombre;
    declaracion.valor = (Funcion*)funcion;
    declaracion.tipo = FUNCION;

    return guardar_declaracion(declaraciones, &declaracion);
}

int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
                            char* nombre_funcion, char* string_lista,
                            int in_place, Declaraciones declaraciones) {
    *funcion = (Funcion*)obtener_def_usuario(declaraciones, nombre_funcion, FUNCION);

    if (in_place && verificar_lista(string_lista))
        *lista = strlist_to_lista(string_lista);
    else
        *lista = (Lista*)obtener_def_usuario(declaraciones, string_lista, LISTA);

    if (*funcion && *lista)
        return 1;
    return 0;
}

int aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones) {
    Lista* list_temp = copiar_lista(lista);
    int status = aplicar_funcion_interno(funcion, list_temp, 0, declaraciones);

    if (status == 0) {
        printf("[ ");
        visitar_lista(list_temp);
        printf("]");
        printf("\n");
    }

    destruir_lista(list_temp);
    return status;
}

// 0 si bien, 1 si hubo error de aplicacion (eliminacion o sucesor a [] / repeticion a largo <2), -1 si hubo overflow
static int aplicar_funcion_interno(Funcion* funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones) {
    Pila* pila_repeticiones = pila_crear();
    unsigned int cantidad = cantidad_composiciones(funcion);
    int status = 0;

    for (unsigned int i = 0; i < cantidad && status == 0 && acc_overflow < MAX_OPERACIONES; i++, acc_overflow++) {
        char* f = funcion_iesima(funcion, i);
        /* Chequeo si es factible comenzar la repetición sino la salteo */
           // Meter en doc capaz.
        if (strcmp(f, "<") == 0)
            if (lista_longitud(lista) && primer_elemento(lista) != ultimo_elemento(lista))
                pila_push(pila_repeticiones, i);
            else
                i = saltear_repeticion(funcion, i);
        else if (strcmp(f, ">") == 0)
            // Vuelvo a chequear si es factible, si no lo es, hago pop y sigo de largo, asi no tengo
            // el costo lineal de saltear_repeticion() hasta este '>' en el caso que ya no haga falta repetir.
            if (lista_longitud(lista) && primer_elemento(lista) != ultimo_elemento(lista)) {
                unsigned int* pos_apertura_rep = pila_top(pila_repeticiones);
                i = *pos_apertura_rep;
            }
            else
                pila_pop(pila_repeticiones);
        else
            status = aplicar_base_o_rebuscar(f, lista, acc_overflow, declaraciones);
    }

    pila_destruir(pila_repeticiones);
    if (acc_overflow == MAX_OPERACIONES)
        status = -1;
    return status;
}

static int aplicar_base_o_rebuscar(char* nombre_funcion, Lista* lista, int acc_overflow, Declaraciones declaraciones) {
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
            return aplicar_funcion_interno(funcion, lista, acc_overflow + 1, declaraciones);
    }
    return valido;
}

// Recibe el i en la posicion del <, tiene que devolverlo en la posicion del > ya que el for hará ++ despues.
static unsigned int saltear_repeticion(Funcion* funcion, unsigned int i) {
    int nivel = 1; // Arranco en 1, teniendo en cuenta al '<' por el que acabo de entrar a esta funcion PASAR A DOCUMENTACION
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

static TipoFuncion str_a_tipo(char* nombre_funcion) {
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
