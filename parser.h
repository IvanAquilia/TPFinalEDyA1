#ifndef PARSER_H
#define PARSER_H

typedef enum {
    OP_INVALIDA,
    OP_DEFF,
    OP_DEFL,
    OP_APPLY,
    OP_SEARCH,
    OP_EXIT,
} TipoOperacion;

typedef struct {
    char* nombre_funcion;  // Nombre funcion a aplicar
    char* string_lista;    // Nombre de lista, o lista in-place ([1,2,3...]).
    int in_place;
} ApplyParseado;

typedef struct {
    char* identificador; // Nombre de función o lista
    void* expresion;     // Definición o expresión asociada
} DefParseado;

typedef struct {
    TipoOperacion tipo;
    void* expresion_parseada;
} ResultadoParser;

/**
 * Dada la linea ingresada por el usuario
 */
ResultadoParser parser_analizar(const char* linea);

/*
 */
int verificar_lista(char* lista_str);

/*
 */
void parser_liberar(ResultadoParser* r);
#endif
