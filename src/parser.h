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
    FUNCION_INEXISTENTE,
    SEARCH_INVALIDO,
} TipoRespuestaParser;

/*
 * Estructura que representa cualquier tipo de respuesta emitida por el parser.
 * Dado que todas menos 1 instruccion (search) se componen de una parte izquierda y una
 * parte derecha (defl izq der | deff izq der | apply izq der) puedo generalizarlo y guardar
 * dichos valores siempre aca. La parte izquierda siempre será un char*, sin embargo por mas que
 * la parte derecha en apply tambien sea un char*, va ser una Lista* o Funcion* o SearchExpr*
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
    TipoRespuestaParser tipo;
} ResultadoParser;

/*
 * Dada la linea ingresada por el usuario, realiza todas las verificaciones necesarias para determinar
 * si se trata de una sentencia valida, si asi lo es, llama a las funciones encargadas de transformar las
 * strings a los objetos en memoria correspondientes. Luego retorna toda la información recolectada en una
 * estructura generica valida para todas las operaciones, identificada mediante el campo "Tipo".
 */
ResultadoParser parser_analizar(const char* linea, Declaraciones declaraciones);

/*
 * Devuelve 1 si la string coincide con un formato de lista valida, sino 0.
 */
int verificar_lista(char* lista_str);

/*
 * Devuelve 1 si la string coincide con un formato de lista valida, sino 0.
 */
int verificar_funcion(char* funcion_str);

/*
 * Devuelve 1 si la string coincide con un formato de expresion de search valida, sino 0.
 */
int verificar_search(char* cursor);

/*
 * Libera los datos que temporalmente se guardaron en los campos de la respuesta antes de guardarse en la tabla hash.
 * La estructura queda lista para volver a utilizarse en una posible siguiente llamada al parser.
 */
void parser_liberar(ResultadoParser* r);
#endif
