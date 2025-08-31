#include "string_utils.h"
#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// ------ Función hash djb2, hashea nombre del objeto que se esta definiendo (Funcion / Lista) ------
unsigned long hash_declaracion(const Declaracion* dato) { // recibe dato NO NULO
    char* nombre = dato->nombre;

    unsigned long h = 5381;
    int c;
    while ((c = *nombre++))
        h = ((h << 5) + h) + c;
    return h;
}

void avanzar_hasta_noespacio(char** cadena) {
    while (isspace((unsigned char)**cadena)) {
        (*cadena)++;
    }
}

void avanzar_hasta_espacio_o_repeticion(char** cadena) {
    while (**cadena &&
           !isspace((unsigned char)**cadena) &&
           **cadena != '<' &&
           **cadena != '>') {
        (*cadena)++;
           }
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
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0')
        return s;

    char* final = s + strlen(s) - 1;
    while (final > s && isspace((unsigned char)*final)) final--;
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
