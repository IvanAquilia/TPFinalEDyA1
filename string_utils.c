#include "string_utils.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// ------ Función hash djb2, hashea nombre del objeto que se esta definiendo (Funcion / Lista) ------
unsigned long hash_clave(const char* string) { // recibe dato NO NULO
    unsigned long h = 5381;
    int c;
    while ((c = *string++))
        h = ((h << 5) + h) + c;
    return h;
}

void avanzar_hasta_noespacio(char** cursor) {
    while (isspace((unsigned char)**cursor))
        (*cursor)++;
}

void avanzar_hasta_espacio_o_repeticion(char** cursor) {
    while (**cursor && !isspace((unsigned char)**cursor) &&
           **cursor != '<' && **cursor != '>')
        (*cursor)++;
}

void avanzar_hasta_nodigito(char** cursor) {
    while (isdigit((unsigned char)**cursor))
        (*cursor)++;
}

void avanzar_hasta_digito(char** cursor) {
    while (**cursor && !isdigit((unsigned char)**cursor))
        (*cursor)++;
}

char* str_dup(const char* original) {
    if (!original)
        return NULL;

    char* s = malloc(sizeof(char) * (strlen(original) + 1));
    assert(s != NULL);
    strcpy(s, original);
    return s;
}

char* str_trim(char* s) {
    if (!s)
        return s;

    avanzar_hasta_noespacio(&s);
    if (*s == '\0')
        return s;

    char* final = s + strlen(s) - 1;
    while (final > s && isspace((unsigned char)*final))
        final--;
    final[1] = '\0';

    return s;
}

int formato_alfanumerico(const char* cadena) {
    int formato_correcto = 1;
    while (*cadena && formato_correcto) {
        if (!isalnum(*cadena))
            formato_correcto = 0;
        cadena++;
    }

    return formato_correcto;
}
