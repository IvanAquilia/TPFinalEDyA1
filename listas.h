#ifndef LISTAS_H
#define LISTAS_H
#include "glist.h"
#include "utils.h"

/**
 * Las listas definidas por el usuario son representadas mediante
 * listas doblemente enlazadas generales.
 */
typedef GList Lista;

/*
 */
Lista* strlist_to_lista(char* cadena);

/*
 */
void lista_insertar_entero(Lista* lista, int* entero);

/*
 * Necesitada al destruir una Declaracion
 */
void destruir_lista(Lista* lista);

/*
 * Necesitada al copiar una Declaracion
 */
Lista* copiar_lista(const Lista* lista);

/*
 * Necesitada al visitar una Declaracion
 */
void visitar_lista(const Lista* lista);

/*
 */
void definir_lista(char* nombre, void* lista, Declaraciones declaraciones);
#endif
