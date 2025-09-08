#ifndef LISTAS_H
#define LISTAS_H
#include "glist.h"

/*
 * Las listas definidas por el usuario son representadas mediante
 * listas de numeros naturales doblemente enlazadas, mediante la interfaz GList.
 */
typedef struct {
    GList* glist;
} Lista;


/*
 * Ya recibe lista verificada que es minimo una [] y que tiene digitos naturales validos dentro,
 * Lee la string en dicho formato y por cada numero que obtiene, lo convierte en un natural para insertarlo
 * en la GList interna.
 * Modifica la cadena original, no hay problema, es el ultimo eslabón del parser.
 */
Lista* strlist_to_lista(char* cadena);

/*
 * Crea una lista, con su GList correspondiente.
 */
Lista* lista_crear();

/*
 * Inserta un natural al final de la lista del usuario.
 */
void lista_insertar_numero_derecha(Lista* lista, unsigned int natural);

/*
 * Inserta un natural al inicio de la lista del usuario.
 */
void lista_insertar_numero_izquierda(Lista* lista, unsigned int natural);

/*
 * Aplica la funcion sucesor al ultimo elemento de la lista del usuario.
 */
void lista_aumentar_derecha(Lista* lista);

/*
 * Aplica la funcion sucesor al primer elemento de la lista del usuario.
 */
void lista_aumentar_izquierda(Lista* lista);

/*
 * Elimina el ultimo elemento de la lista del usuario. Siempre se llama con listas no vacias.
 */
void lista_eliminar_derecha(Lista* lista);

/*
 * ELimina el primer elemento de la lista del usuario. Siempre se llama con listas no vacias.
 */
void lista_eliminar_izquierda(Lista* lista);

/*
 * Destruye la lista del usuario.
 */
void destruir_lista(Lista* lista);

/*
 * Determina si se trata de la lista vacia.
 */
int lista_vacia(Lista* lista);

/*
 * Devuelve la cantidad de numeros naturales en la lista.
 */
unsigned int lista_longitud(const Lista* lista);

/*
 * Devuelve un puntero al primer elemento de la lista, si es la lista vacia devuelve NULL.
 */
unsigned int* primer_elemento(Lista* lista);

/*
 * Devuelve un puntero al ultimo elemento de la lista, si es la lista vacia devuelve NULL.
 */
unsigned int* ultimo_elemento(Lista* lista);

/*
 * Realiza una copia profunda de la lista.
 */
Lista* copiar_lista(const Lista* lista);

/*
 * Devuelve 1 si las listas son iguales elemento por elemento, sino 0.
 */
int listas_iguales(Lista* lista1, Lista* lista2);

/*
 * Necesitada al visitar una Declaracion
 */
void visitar_lista(const Lista* lista);

/*
 * Variante de la funcion hash FNV-1a, adaptada para hashear mi lista de naturales y diferenciar por longitud.
 * Funciona bien con secuencias de enteros cortas, es rapida y altamente testeada y documentada.
 */
unsigned long hash_lista(const Lista* lista);
#endif
