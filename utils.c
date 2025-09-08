#include "utils.h"
#include "listas.h"
#include "funciones.h"
#include "string_utils.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define CANTIDAD_DECLARACIONES 10007 // Primo


// -------------- Funciones de comparacion, copia... etc. pasadas a las estructuras -------------- //

/* ------------- STRINGS ------------- */
int cmp_str(const char* a, const char* b) {
    return strcmp(a, b);
}

void destruir_str(char* dato) {
    free(dato);
}

char* copiar_str(const char* dato) {
    char* nuevo = malloc(sizeof(char) * strlen(dato) + 1);
    assert(nuevo != NULL);
    strcpy(nuevo, dato);
    return nuevo;
}

void visitar_str(const char* dato) {
    printf("%s ", dato);
}

/* ------------- NUMEROS NATURALES ------------- */
int cmp_uint(const unsigned int* a, const unsigned int* b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

void destruir_uint(unsigned int* natural) {
    free(natural);
}

unsigned int* copiar_uint(const unsigned int* natural) {
    unsigned int* nuevo = malloc(sizeof(unsigned int));
    *nuevo = *natural;
    return nuevo;
}

void visitar_uint(const unsigned int* natural) {
    printf("%u", *natural);
}

/* ------------- DECLARACIONES ------------- */
int comparar_declaracion(const Declaracion* declaracion1, const Declaracion* declaracion2) {
    /* 2 declaraciones nunca van a ser iguales mediante su nombre, la tabla hash no lo permite por diseño.
     * Asi que decimos que son iguales por el contenido. */
    int iguales = -1;
    if (declaracion1->tipo == declaracion2->tipo) {
        if (declaracion1->tipo == FUNCION) {
            if (funciones_iguales(declaracion1->valor, declaracion2->valor))
                iguales = 0;
        }
        if (declaracion1->tipo == LISTA) {
            if (listas_iguales(declaracion1->valor, declaracion2->valor))
                iguales = 0;
        }
    }

    return iguales;
}

void destruir_declaracion(Declaracion* declaracion) {
    free(declaracion->nombre);
    if (declaracion->tipo == LISTA)
        destruir_lista((Lista*)declaracion->valor);
    else
        destruir_funcion((Funcion*)declaracion->valor);

    free(declaracion);
}

Declaracion* copiar_declaracion(const Declaracion* declaracion) {
    Declaracion* nueva_declaracion = malloc(sizeof(Declaracion));
    assert(nueva_declaracion != NULL);

    nueva_declaracion->nombre = malloc(strlen(declaracion->nombre) + 1);
    assert(nueva_declaracion->nombre != NULL);
    strcpy(nueva_declaracion->nombre, declaracion->nombre);

    nueva_declaracion->tipo = declaracion->tipo;
    if (declaracion->tipo == LISTA) {
        nueva_declaracion->valor = copiar_lista(declaracion->valor);
    } else {
        nueva_declaracion->valor = copiar_funcion(declaracion->valor);
    }

    return nueva_declaracion;
}

void visitar_declaracion(const Declaracion* declaracion) {
    if (declaracion->tipo == LISTA) {
        printf("LISTA nombrada '%s': ", declaracion->nombre);
        visitar_lista(declaracion->valor);
        printf("\n");
    } else {
        printf("FUNCION nombrada '%s': ", declaracion->nombre);
        visitar_funcion(declaracion->valor);
        printf("\n");
    }
}

int guardar_declaracion(Declaraciones declaraciones, Declaracion* declaracion) {
    int guardada = tabla_hash_insertar(declaraciones, declaracion->nombre, declaracion);
    return guardada;
}

Declaraciones declaraciones_crear() {
    Declaraciones declaraciones = tabla_hash_crear(
        CANTIDAD_DECLARACIONES,
        (FuncionHash)hash_identificador,
        (FuncionComparadora)cmp_str,
        (FuncionDestructora)destruir_declaracion,
        (FuncionDestructora)destruir_str,
        (FuncionCopia)copiar_declaracion,
        (FuncionCopia)copiar_str,
        (FuncionVisitante)visitar_declaracion);
    return declaraciones;
}

void destruir_declaraciones(Declaraciones declaraciones) {
    tabla_hash_destruir(declaraciones);
}

/* ------------- ESTADOS DE LISTA ------------- */

unsigned long hash_estado(const EstadoLista* estado) {
    unsigned long h = hash_lista(estado->lista);
    h ^= estado->profundidad + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
    return h;
}

int cmp_estado(const EstadoLista* a, const EstadoLista* b) {
    if (a->profundidad == b->profundidad && listas_iguales(a->lista, b->lista))
        return 0;

    return -1;
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

void visitar_estado(const unsigned int* dummy) {
    printf("Valor dummy de estado en tabla: %u\n", *dummy);
}

/* ------------- AUXILIARES/VARIAS ------------- */
int definir(TipoDeclaracion tipo, char* nombre, void* defincion, Declaraciones declaraciones) {
    Declaracion declaracion;
    declaracion.nombre = nombre;
    declaracion.valor = defincion;
    declaracion.tipo = tipo;

    return guardar_declaracion(declaraciones, &declaracion);
}

void* obtener_def_usuario(Declaraciones declaraciones, const void* clave, TipoDeclaracion tipo) {
    Declaracion* declaracion = tabla_hash_buscar(declaraciones, clave);
    if (declaracion && declaracion->tipo == tipo)
        return declaracion->valor;
    return NULL;
}

int generar_funciones_base(Declaraciones declaraciones) {
    char funciones_base[6][3] = {
        "Oi",
        "Od",
        "Si",
        "Sd",
        "Di",
        "Dd"
    };
    int todas_generadas = 1;

    for (int i = 0; i < 6 && todas_generadas; i++) {
        Funcion* funcion = funcion_crear();
        componer_funcion(funcion, funciones_base[i]);
        if (!definir(FUNCION, funciones_base[i], funcion, declaraciones))
            todas_generadas = 0;
        destruir_funcion(funcion); // Una vez transferida la propiedad del elemento a la
                                   // tabla hash (con un deep-copy) destruyo esta version.
    }

    return todas_generadas;
}

int es_primo(unsigned int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    unsigned int limite = (unsigned int)sqrt(n);
    for (unsigned int i = 3; i <= limite; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

unsigned int siguiente_primo(unsigned int n) {
    unsigned int candidato = n + 1;
    while (!es_primo(candidato)) {
        candidato++;
    }
    return candidato;
}

