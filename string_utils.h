#ifndef STRING_UTILS_H
#define STRING_UTILS_H


/*
 */
unsigned long hash_clave(const char* string);

/*
 */
void avanzar_hasta_noespacio(char** cursor);

/*
 */
void avanzar_hasta_espacio_o_repeticion(char** cursor);


/*
 */
void avanzar_hasta_pcoma(char** cursor);

/*
 */
void avanzar_hasta_nodigito(char** cursor);

/*
 */
void avanzar_hasta_digito(char** cursor);

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
