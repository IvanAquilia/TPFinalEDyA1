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

/* Utils strings */
int cmp_str(const char* a, const char* b);
void destruir_str(char* dato);
char* copiar_str(const char* dato);
void visitar_str(const char* dato);

/* Utils numeros naturales */
int cmp_uint(const unsigned int* a, const unsigned int* b);
void destruir_uint(unsigned int* dato);
unsigned int* copiar_uint(const unsigned int* dato);
void visitar_uint(const unsigned int* dato);

/* Utils declaraciones */
int comparar_declaracion(const Declaracion* declaracion1, const Declaracion* declaracion2);
void destruir_declaracion(Declaracion* declaracion);
Declaracion* copiar_declaracion(const Declaracion* declaracion);
void visitar_declaracion(const Declaracion* declaracion);
int guardar_declaracion(Declaraciones declaraciones, Declaracion* declaracion);
Declaraciones declaraciones_crear();
void destruir_declaraciones(Declaraciones declaraciones);

/* Auxiliares */

/*
 * Dado un tipo de definicion (Lista/Funcion), el identificador correspondiente y la propia definicion,
 * encapsula los datos en una Declaracion e inserta dicho objeto en la tabla de Declaraciones.
 */
int definir(TipoDeclaracion tipo, char* nombre, void* lista, Declaraciones declaraciones);

/*
 * Dado el nombre de la declaracion y el tipo buscado, obtiene su valor en la tabla hash de Declaraciones.
 * A cargo del caller el casteo del retorno segun el tipo.
 */
void* obtener_def_usuario(Declaraciones declaraciones, const void* clave, TipoDeclaracion tipo);

/*
 * Las funciones base son como si se hiciera un hipotetico "deff Di = Di", "deff Od = Od"... etc
 * Guardo una string de sí misma dentro del array porque en el sistema, por integridad,
 * no se permiten funciones nulas, de este modo a la hora de aplicar una funcion reutilizo la logica de
 * buscar por nombre y, en el caso que no sean funciones base, llamo recursivamente hasta llegar a una.
 * Esto permite que el usuario pueda hacer aplicaciones directas del estilo:
 * "apply Sd L1" y reutilizar el codigo ya escrito para funciones custom.
 *
 * (Ver funcion aplicar_funcion() para ver en accion la naturalidad de esta decisión)
 */
int generar_funciones_base(Declaraciones declaraciones);

int es_primo(unsigned int n);
unsigned int siguiente_primo(unsigned int n);
#endif
