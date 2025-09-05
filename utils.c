#include "utils.h"
#include "listas.h"
#include "funciones.h"
#include "string_utils.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#define CANTIDAD_DECLARACIONES 10007

// -------------- Funciones de comparacion, copia... etc. pasadas a las estructuras -------------- //

// ----------- STRINGS
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

// ----------- NATURALES
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
    printf("%u ", *natural);
}

// ----------- DECLARACIONES
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
    // Transfiero la propiedad de la Declaracion hacia la memoria que maneja la tabla hash generica
    // internamente, genero una copia y guardo en la tabla hash la declaracion "nombre: lista/funcion".
    int guardada = tabla_hash_insertar(declaraciones, declaracion->nombre, declaracion);
    return guardada;
}

Declaraciones declaraciones_crear() {
    Declaraciones declaraciones = tabla_hash_crear(CANTIDAD_DECLARACIONES,
                                                    (FuncionHash)hash_clave,
                                                    (FuncionComparadora)cmp_str,
                                                    (FuncionDestructora)destruir_declaracion,
                                                    (FuncionDestructora)destruir_str,
                                                    (FuncionCopia)copiar_declaracion,
                                                    (FuncionCopia)copiar_str,
                                                    (FuncionVisitante)visitar_declaracion);
    return declaraciones;
}

// ----------- AUXILIARES/VARIAS

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

    // Las funciones base son como si se hiciera un hipotetico "deff Di = Di", "deff Od = Od"... etc
    // Guardo una string de sí misma dentro del array porque en el sistema por integridad
    // no se permiten funciones nulas, de este modo, a la hora de aplicar una funcion,
    // reutilizo la logica de buscar por nombre y, en el caso que
    // no sean funciones base, llamar recursivamente.
    // Esto permite que el usuario pueda hacer aplicaciones directas del estilo:
    // "apply Sd L1" y reutilizar el codigo ya escrito para funciones custom.
    // (Ver funcion aplicar_funcion() para ver en accion la naturalidad de esta decisión)
    for (int i = 0; i < 6 && todas_generadas; i++) {
        Funcion* funcion = funcion_crear();
        componer_funcion(funcion, funciones_base[i]);
        if (!definir_funcion(funciones_base[i], funcion, declaraciones))
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

