#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "garray.h"
#include "listas.h"

typedef enum {
    Oi,
    Od,
    Si,
    Sd,
    Di,
    Dd,
    Custom
} TipoFuncion;

/*
 * Cada funcion es un array dinamico de los nombres (char*) de las funciones que la compone en orden.
 * Ej. : f1: ["0d", "Mi", "<Dd>", "Si"]
 */
typedef struct {
    GArray* garray;
} Funcion;

/*
 */
Funcion* strfunc_to_array(char* cadena, Declaraciones declaraciones);

/*
 */
Funcion* funcion_crear();

/*
 */
void componer_funcion(Funcion* funcion, char* nombre);

/*
 * Necesitada al destruir una Declaracion
 */
void destruir_funcion(Funcion* funcion);

/*
 * Necesitada al copiar una Declaracion
 */
Funcion* copiar_funcion(const Funcion* funcion);

/*
 * Necesitada al visitar una Declaracion
 */
void visitar_funcion(const Funcion* funcion);

/*
 */
unsigned int cantidad_composiciones(const Funcion* funcion);

/*
 */
char* funcion_iesima(Funcion* funcion, unsigned int i);

/*
 */
int definir_funcion(char* nombre, void* funcion, Declaraciones declaraciones);

/*
 */
int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
                            char* nombre_funcion, char* string_lista,
                            int in_place, Declaraciones declaraciones);

/*
 */
int aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones);


#endif
