#ifndef STRING_UTILS_H
#define STRING_UTILS_H
#include "utils.h"


/*
 */
unsigned long hash_declaracion(const Declaracion* dato);

/*
 */
void avanzar_hasta_noespacio(char** cadena);

/*
 */
void avanzar_hasta_espacio_o_repeticion(char** cadena);

/*
 */
char* str_dup(const char* original);

/*
 */
char* str_trim(char* s);

/*
 */
int formato_alfanumerico(const char* cadena);

#endif
