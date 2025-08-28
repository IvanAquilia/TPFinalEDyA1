#ifndef LISTAS_H
#define LISTAS_H
#include "glist.h"

/**
 * Las listas definidas por el usuario son representadas mediante
 * listas doblemente enlazadas generales.
 */
typedef GList* Lista;

Lista strlist_to_lista(char* cadena);

int cmp_int(const int* a, const int* b);

void destruir_int(int* dato);

int* copiar_int(const int* dato);

void visitar_int(const int* dato);

#endif
