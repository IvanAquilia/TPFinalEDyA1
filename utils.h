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
int cmp_int(const int* a, const int* b);
void destruir_int(int* dato);
int* copiar_int(const int* dato);
void visitar_int(const int* dato);

/*
 * Utils declaracion/declaraciones
 */
Declaracion* declaracion_crear(TipoDeclaracion tipo, char* nombre, void* valor);
int cmp_declaracion(const Declaracion* a, const Declaracion* b);
void destruir_declaracion(Declaracion* declaracion);
Declaracion* copiar_declaracion(const Declaracion* declaracion);
void visitar_declaracion(const Declaracion* declaracion);
Declaraciones declaraciones_crear();


/*
 * Auxiliares
 */
void declarar_y_manejar_output(Declaraciones declaraciones, Declaracion* declaracion);

#endif
