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
    TipoOperacion tipo;
    char* identificador;   // Nombre de función o lista
    void* expresion;       // Definición o expresión asociada
} ResultadoParser;

/**
 * Dada la linea ingresada por el usuario
 */
ResultadoParser parser_analizar(const char* linea);

/*
 */
void parser_liberar(ResultadoParser* r);
#endif
