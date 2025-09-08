#include "parser.h"
#include "string_utils.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/*
 * Avanza puntero hasta el comienzo de lo que se encuentre luego del signo igual, omitiendo todos los espacios
 * en el medio, luego devuelve dicha string encontrada previa al signo igual, si fue posible.
 */
static char* obtener_cadena_pre_igual(char** cursor);

/*
 * Caso similar pero especial al anterior, cuando se trata de un apply, obtiene el nombre de la funcion
 * previo al espacio que indica el comienzo de la lista. No realiza tantas verificaciones como la anterior.
 */
static char* obtener_nombre_funcion(char** cursor);

/*
 * Chequea que se trate de un caracter válido posible dentro de la definicion de una funcion.
 */
static int identificador_invalido(char caracter);

/*
 * Wrapper para mas verbosidad de chequeo de una repeticion vacía '<>'
 */
static int repeticion_vacia(char caracter_1, char caracter_2);

/*
 * Asigna los objetos parseados (listas, funciones, expresiones search) a los campos del ResultadoParser.
 */
static void terminar_parseo(ResultadoParser* r, char* izq, void* der, int in_place, TipoRespuestaParser tipo);


ResultadoParser parser_analizar(const char* input, Declaraciones declaraciones) {
    ResultadoParser r;
    r.in_place = 0;
    r.parte_izquierda = NULL;
    r.parte_derecha = NULL;
    r.tipo = OP_INVALIDA;

    // Lo copio para no modificarlo
    char* buffer = str_dup(input);
    char* cursor = buffer;
    cursor[strcspn(cursor, "\n")] = '\0'; // Tomo hasta primer '\n'
    cursor = str_trim(cursor);
    size_t len = strlen(cursor);

    if (len != 0 && cursor[len-1] == ';') {
        cursor[len-1] = '\0';  // Elimino el '; '

        if (strcmp(cursor, "exit") == 0) {
            r.tipo = OP_EXIT;
        }
        else if (strncmp(cursor, "defl ", 5) == 0) {
            char* nombre = obtener_cadena_pre_igual(&cursor);
            if (nombre && *cursor) {
                int valida = verificar_lista(cursor);
                if (valida) {
                    Lista* l = strlist_to_lista(cursor);
                    if (l != NULL) {
                        nombre = str_dup(nombre); // Duplico porque al final del parser
                                                  // libero buffer y "nombre" era un pedacito
                                                  // de buffer, no era un char* nuevo.
                        terminar_parseo(&r, nombre, l, 0, OP_DEFL);
                    } else {
                        r.tipo = OVERFLOW_LISTA;
                    }
                }
            }
        } else if (strncmp(cursor, "deff ", 5) == 0){
            char* nombre = obtener_cadena_pre_igual(&cursor);
            if (nombre && *cursor) {
                int valida = verificar_funcion(cursor);
                if (valida) {
                    Funcion* f = strfunc_to_funcion(cursor, declaraciones);
                    if (f != NULL) {
                        nombre = str_dup(nombre); // Duplico por mismo motivo
                        terminar_parseo(&r, nombre, f, 0, OP_DEFF);
                    } else {
                        r.tipo = FUNCION_INEXISTENTE;
                    }
                }
            }
        } else if (strncmp(cursor, "apply ", 6) == 0) {
            char* nombre_funcion = obtener_nombre_funcion(&cursor);
            if (nombre_funcion && *cursor) {
                char* nombre_lista = cursor;
                nombre_lista = str_trim(nombre_lista);
                nombre_lista = str_dup(nombre_lista); // Duplico por mismo motivo
                nombre_funcion = str_dup(nombre_funcion);
                int in_place = *nombre_lista == '[' ? 1 : 0; // Puede tratarse de una lista in-place
                terminar_parseo(&r, nombre_funcion, nombre_lista, in_place, OP_APPLY);
            }
        } else if (strncmp(cursor, "search ", 7) == 0) {
            int valida = verificar_search(cursor);
            if (valida) {
                SearchExpr* search = strsearch_to_search(cursor, declaraciones);
                if (search != NULL) {
                    terminar_parseo(&r, NULL, search, 0, OP_SEARCH);
                } else {
                    r.tipo = SEARCH_INVALIDO;
                }
            }
        }
    }

    free(buffer);
    return r;
}

int verificar_search(char* cursor) {
    cursor += 7;
    avanzar_hasta_noespacio(&cursor);
    char* fin_search = strchr(cursor, '}');
    int valida = 1;
    char ultimo = ';'; // Arranco esperando una ','
    if (*cursor && *cursor == '{' && fin_search) {
        cursor++;
        avanzar_hasta_noespacio(&cursor);
        if (cursor != fin_search) {  // Prohibir repeticiones vacias
            while (*cursor != *fin_search && valida) {
                if (isalnum(*cursor))
                    while (isalnum(*cursor)) cursor++;
                else
                    valida = 0;

                if ((ultimo == ',' && *cursor == ';') || (ultimo == ';' && *cursor == ',')) {
                    cursor++;
                    avanzar_hasta_noespacio(&cursor);
                    ultimo = ultimo == ';' ? ',' : ';';
                } else {
                    valida = 0;
                }
            }
        } else {
            valida = 0;
        }
    } else {
        valida = 0;
    }

    return valida;
}

int verificar_funcion(char* funcion_str) {
    char* cursor = funcion_str;
    int valida = 1, nivel = 0;

    while (*cursor && valida) {
        if (identificador_invalido(*cursor) || repeticion_vacia(*cursor, *(cursor + 1))) {
            valida = 0;
        } else {
            if (*cursor == '<') {
                nivel++;
            } else if (*cursor == '>') {
                nivel--;
                if (nivel < 0)
                    valida = 0;
            }
            cursor++;
        }
    }

    if (valida && nivel == 0) {
        return 1;
    }
    return 0;
}

int verificar_lista(char* lista_str) {
    char* cursor = lista_str;
    int valida = 1;
    char* corchete_final = strchr(cursor, ']');
    if (*cursor == '[' && corchete_final && *(corchete_final + 1) == '\0') {
        cursor++; // Primer elemento despues del '['
        while (cursor != corchete_final && valida) {
            if (isdigit(*cursor))
                while (isdigit(*cursor)) cursor++;
            else
                valida = 0;

            if (cursor != corchete_final && valida) { //  Si no era el ultimo numero valido...
                if (*cursor == ',' ) {
                    if (*(cursor + 1) == ' ' && isdigit(*(cursor + 2))) // Caso: [1, 2...]
                        cursor += 2;
                    else if (isdigit(*(cursor + 1))) // Caso: [1,2...]
                        cursor++;
                    else
                        valida = 0;
                } else {
                    valida = 0;
                }
            }
        }
    } else {
        valida = 0;
    }
    return valida;
}

void parser_liberar(ResultadoParser* r) {
    if (!r)
        return;

    if (r->tipo == OP_APPLY) {
        free(r->parte_izquierda);
        free(r->parte_derecha);
    } else if (r->tipo == OP_SEARCH) {
        destruir_search_expr((SearchExpr*)r->parte_derecha);
    } else {
        free(r->parte_izquierda);
        if (r->tipo == OP_DEFL) {
            destruir_lista((Lista*)r->parte_derecha);
        } else if (r->tipo == OP_DEFF) {
            destruir_funcion((Funcion*)r->parte_derecha);
        }
    }
    r->in_place = 0;
    r->tipo = OP_INVALIDA;
}

static char* obtener_cadena_pre_igual(char** cursor) {
    *cursor += 5;

    if (**cursor != '=') {  // No se permite al nombre empezar con '='
        avanzar_hasta_noespacio(cursor);
        if (**cursor) {
            char* cadena_pre_igual = *cursor;
            char* igual = strchr(*cursor, '=');
            if (igual) {
                *igual = '\0';
                cadena_pre_igual = str_trim(cadena_pre_igual);
                if (formato_alfanumerico(cadena_pre_igual)) {
                    *cursor = igual + 1;
                    avanzar_hasta_noespacio(cursor);
                    return cadena_pre_igual;
                }
            }
        }
    }
    return NULL;
}

static char* obtener_nombre_funcion(char** cursor) {
    *cursor += 6;
    avanzar_hasta_noespacio(cursor);
    if (**cursor) {
        char* nombre_funcion = *cursor;
        char* espacio = strchr(*cursor, ' ');
        if (espacio) {
            *espacio = '\0';
            *cursor = espacio + 1;
            avanzar_hasta_noespacio(cursor);
            return nombre_funcion;
        }
    }

    return NULL;
}


static int identificador_invalido(char caracter) {
    if (!isalnum(caracter) && caracter != '<' && caracter != '>' && caracter != ' ')
        return 1;
    return 0;
}

static int repeticion_vacia(char caracter_1, char caracter_2) {
    if ((caracter_1 == '<' && caracter_2 == '>'))
        return 1;
    return 0;
}

static void terminar_parseo(ResultadoParser* r, char* izq, void* der, int in_place, TipoRespuestaParser tipo) {
    /* Los apply son el unico caso que tanto la parte izquierda como derecha de
     * la instruccion son strings (nombre de funcion a aplicar | nombre de lista a ser aplicada), se guarda
     * como void* pero internamente puedo luego saber gracias al tipo OP_APPLY que en verdad apunta
     * a un char* duplicado por str_dup en el parser. */

    // En los search, la parte izquierda de char* será NULL, ya que no la hay.
    r->parte_izquierda = izq;
    r->parte_derecha = der;
    r->in_place = in_place;
    r->tipo = tipo;
}

