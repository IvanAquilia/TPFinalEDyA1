#ifndef LISTAS_H
#define LISTAS_H
#include "glist.h"
#include "utils.h"

/**
 * Las listas definidas por el usuario son representadas mediante
 * listas de numeros naturales doblemente enlazadas, mediante la interfaz GList.
 */
typedef struct {
    GList* glist;
} Lista;


/*
 */
Lista* strlist_to_lista(char* cadena);

/*
 */
Lista* lista_crear();

/*
 */
void lista_insertar_numero_derecha(Lista* lista, unsigned int natural);

/*
 */
void lista_insertar_numero_izquierda(Lista* lista, unsigned int natural);

/*
 */
void lista_aumentar_derecha(Lista* lista);

/*
 */
void lista_aumentar_izquierda(Lista* lista);

/*
 */
void lista_eliminar_derecha(Lista* lista);

/*
 */
void lista_eliminar_izquierda(Lista* lista);

/*
 * Necesitada al destruir una Declaracion
 */
void destruir_lista(Lista* lista);

/*
 */
int lista_vacia(Lista* lista);

/*
 */
unsigned int lista_longitud(Lista* lista);

/*
 */
unsigned int* primer_elemento(Lista* lista);

/*
 */
unsigned int* ultimo_elemento(Lista* lista);

/*
 * Necesitada al copiar una Declaracion
 */
Lista* copiar_lista(const Lista* lista);

/*
 */
int listas_iguales(Lista* lista1, Lista* lista2);

/*
 * Necesitada al visitar una Declaracion
 */
void visitar_lista(const Lista* lista);

/*
 */
int definir_lista(char* nombre, void* lista, Declaraciones declaraciones);

/*
 */
unsigned long hash_lista(const Lista* lista);
#endif
