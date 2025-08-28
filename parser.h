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
    char* nombre;   // Nombre de función o lista
    void* expr;     // Definición o expresión asociada
} ResultadoParser;

/**
 * Dada la linea ingresada por el usuario
 */
ResultadoParser parser_analizar(const char* linea);

/*
 */
static char* parsear_nombre(char** buffer);

/*
 */
static int verificar_funcion(char* cursor);

/*
 */
static int verificar_lista(char* corchete_inicio, char* corchete_final);

/*
 */
static int formato_alfanumerico(const char* cadena);

/*
 */
static int identificador_invalido(char caracter);

/*
 */
static int repeticion_vacia(char caracter_1, char caracter_2);

/*
 */
void parser_liberar(ResultadoParser* r);
#endif
