#ifndef STRING_UTILS_H
#define STRING_UTILS_H

/*
 * Función hash djb2 para strings, hashea nombre del objeto que se esta definiendo (Funcion / Lista).
 * Especialmente util y testada para caracteres, es muy livina de ejecutar.
 */
unsigned long hash_clave(const char* string);

/*
 * Dado un puntero a char lo avanza hasta la siguiente posicion donde no haya un espacio en blanco
 */
void avanzar_hasta_noespacio(char** cursor);

/*
 * Dado un puntero a char lo avanza hasta la siguiente posicion donde haya un espacio en blanco, un terminador
 * o un caracteres '<' o '>'.
 */
void avanzar_hasta_espacio_o_repeticion(char** cursor);


/*
 * Dado un puntero a char lo avanza hasta la siguiente posicion donde haya un terminador, o un caracter ';' o un
 * caracter ','.
 */
void avanzar_hasta_pcoma(char** cursor);

/*
 * Dado un puntero a char lo avanza hasta la siguiente posicion donde no haya un digito.
 */
void avanzar_hasta_nodigito(char** cursor);

/*
 * Dado un puntero a char lo avanza hasta la siguiente posicion donde haya un terminador o un digito.
 */
void avanzar_hasta_digito(char** cursor);

/*
 * Dada una string, realiza una copia profunda de la misma.
 */
char* str_dup(const char* original);

/*
 * Elimina los espacios a izquierda y derecha de una string.
 */
char* str_trim(char* s);

/*
 * Chequea si una string completa posee solo caracteres alfanumericos.
 */
int formato_alfanumerico(const char* cadena);

#endif
