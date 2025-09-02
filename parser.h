#ifndef PARSER_H
#define PARSER_H
#include "utils.h"

// Las posibles situaciones en las que puede terminar el parser.
// Generalmente seran potenciales operaciones, de ahí el nombre del enum.
typedef enum {
    OP_INVALIDA,
    OP_DEFF,
    OP_DEFL,
    OP_APPLY,
    OP_SEARCH,
    OP_EXIT,
    OVERFLOW_LISTA,
    FUNCION_INEXISTENTE
} TipoOperacion;

/*
 * Estructura que representa cualquier tipo de respuesta emitida por el parser.
 * Dado que todas menos 1 instruccion (search) se componen de una parte izquierda y una
 * parte derecha (defl izq der | deff izq der | apply izq der) puedo generalizarlo y guardar
 * dichos valores siempre aca. La parte izquierda siempre será un char*, sin embargo por mas que
 * la parte derecha en apply tambien es una string, va ser una Lista* o Funcion* o SearchExpr*
 * en el caso de defl, deff o search, por lo que necesito que sea void*.
 * El campo in_place solo será accedido en el caso que se sepa mediante el tipo que se esta
 * trabajando con un apply.
 * Cuando se trate de un search, se considera la parte izquierda NULL y solo se accederá a
 * la parte derecha, nuevamente determinado por el tipo.
 */
typedef struct {
    char* parte_izquierda;
    void* parte_derecha;
    int in_place;
    TipoOperacion tipo;
} ResultadoParser;

/**
 * Dada la linea ingresada por el usuario
 */
ResultadoParser parser_analizar(const char* linea, Declaraciones declaraciones);

/*
 */
int verificar_lista(char* lista_str);

/*
 */
void parser_liberar(ResultadoParser* r);
#endif
