#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "garray.h"
#include "listas.h"
#include "utils.h"

/*
 * Cada funcion es un array dinamico de los nombres (char*) de las funciones que la compone en orden.
 * Ej. : f1: ["0d", "Mi", "<Dd>", "Si"]
 */

typedef GArray Funcion;

/*
 */
Funcion* strfunc_to_array(char* cadena);

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
int definir_funcion(char* nombre, void* funcion, Declaraciones declaraciones);

/*
 */
void aplicar_funcion(Funcion* funcion, Lista* lista);

/*
 */
int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
                            char* nombre_funcion, char* string_lista,
                            int in_place, Declaraciones declaraciones);

/*
 */
void generar_funciones_base(Declaraciones declaraciones);

#endif
