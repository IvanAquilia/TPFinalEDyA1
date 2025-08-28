#ifndef UTILS_H
#define UTILS_H

typedef enum {
    PRIMITIVA,
    COMPUESTA,
    REPETICION
} TipoFuncion;

typedef enum {
    LISTA,
    FUNCION,
} TipoDeclaracion;

typedef enum {
    OI, OD, SI, SD, DI, DD
} Operacion;

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

/*
 */
char* str_dup(const char* original);

/*
 */
void avanzar_hasta_noespacio(char** cadena);

/*
 */
void avanzar_hasta_espacio_o_repeticion(char** cadena);

/*
 */
char* str_trim(char* s);

int cmp_str(const char* a, const char* b);

void destruir_str(char* dato);

char* copiar_str(const char* dato);

void visitar_str(const char* dato);

int cmp_int(const int* a, const int* b);

void destruir_int(int* dato);

int* copiar_int(const int* dato);

void visitar_int(const int* dato);

#endif
