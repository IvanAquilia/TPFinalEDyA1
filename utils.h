#ifndef UTILS_H
#define UTILS_H
#include "hash.h"

typedef enum {
    LISTA,
    FUNCION,
} TipoDeclaracion;

/**
 * Estructura que representa las declaraciones tanto
 * de listas como de funciones del usuario. Es lo que habrá en cada
 * casilla de la tabla hash utilizada.
 */
typedef struct {
    TipoDeclaracion tipo;
    char* nombre;
    void* valor;
} Declaracion;

typedef HashTable* Declaraciones;

/*
 * Utils strings
 */
int cmp_str(const char* a, const char* b);
void destruir_str(char* dato);
char* copiar_str(const char* dato);
void visitar_str(const char* dato);

/*
 * Utils enteros
 */
int cmp_uint(const unsigned int* a, const unsigned int* b);
void destruir_uint(unsigned int* dato);
unsigned int* copiar_uint(const unsigned int* dato);
void visitar_uint(const unsigned int* dato);

/*
 * Utils declaracion/declaraciones
 */
int comparar_declaracion(const Declaracion* declaracion1, const Declaracion* declaracion2);
void destruir_declaracion(Declaracion* declaracion);
Declaracion* copiar_declaracion(const Declaracion* declaracion);
void visitar_declaracion(const Declaracion* declaracion);
int guardar_declaracion(Declaraciones declaraciones, Declaracion* declaracion);
Declaraciones declaraciones_crear();


/*
 * Auxiliares
 */
void* obtener_def_usuario(Declaraciones declaraciones, const void* clave, TipoDeclaracion tipo);
int generar_funciones_base(Declaraciones declaraciones);

int es_primo(unsigned int n);
unsigned int siguiente_primo(unsigned int n);
#endif
