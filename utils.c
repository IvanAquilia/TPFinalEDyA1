#include "utils.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// ------ Función hash djb2, hashea nombre del objeto que se esta definiendo (Funcion / Lista) ------
unsigned long hash_par(const Declaracion* dato) {
    const char* nombre = dato->nombre;
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

int cmp_str(const char* a, const char* b) {
    return strcmp(a, b);
}

void destruir_str(char* dato) {
    free(dato);
}

char* copiar_str(const char* dato) {
    char* nuevo = malloc(sizeof(char) * strlen(dato) + 1);
    strcpy(nuevo, dato);
    return nuevo;
}

void visitar_str(const char* dato) {
    printf("%s ", dato);
}

int cmp_int(const int* a, const int* b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

void destruir_int(int* dato) {
    free(dato);
}

int* copiar_int(const int* dato) {
    int* nuevo = malloc(sizeof(int));
    *nuevo = *dato;
    return nuevo;
}

void visitar_int(const int* dato) {
    printf("%d ", *dato);
}
