#include "parser.h"
#include "listas.h"
#include "funciones.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Avanza puntero hasta el comienzo de la definicion y luego devuelve el identificador de dicha definicion, si es posible.
 */
static char* parsear_nombre(char** buffer);
static int verificar_funcion(char* buffer);
static int verificar_lista(char* corchete_inicio, char* corchete_final);
static int identificador_invalido(char caracter);
static int repeticion_vacia(char caracter_1, char caracter_2);

ResultadoParser parser_analizar(const char* input) {
    ResultadoParser r;
    r.expresion = NULL;
    r.identificador = NULL;
    r.tipo = OP_INVALIDA;

    // Lo copio para no modificarlo
    char* buffer = str_dup(input);
    char* cursor = buffer;
    cursor[strcspn(cursor, "\n")] = '\0'; // Tomo hasta primer '\n'
    cursor = str_trim(cursor);
    size_t len = strlen(cursor);

    if (len != 0 || cursor[len-1] == ';') {
        cursor[len-1] = '\0';             // Elimino el '; '

        if (strcmp(cursor, "exit") == 0) {
            r.tipo = OP_EXIT;
        }
        else if (strncmp(cursor, "defl ", 5) == 0) {
            char* nombre = parsear_nombre(&cursor);
            if (nombre && *cursor && *cursor == '[') {
                char* corchete_inicio = cursor;
                char* corchete_final = strchr(cursor, ']');
                if (corchete_inicio && corchete_final && *(corchete_final + 1) == '\0') {
                    int invalido = verificar_lista(corchete_inicio, corchete_final);
                    if (!invalido) {
                        char* lista = corchete_inicio;
                        Lista* l = strlist_to_lista(lista);
                        if (l != NULL) {
                            r.identificador = str_dup(nombre); // Duplico porque libero buffer despues
                            r.expresion = l;
                            r.tipo = OP_DEFL;
                        }
                    }
                }
            }
        } else if (strncmp(cursor, "deff ", 5) == 0){
            char* nombre = parsear_nombre(&cursor);
            if (nombre && *cursor) {
                int invalido = verificar_funcion(cursor);
                if (!invalido) {
                    Funcion* f = strfunc_to_array(cursor);
                    if (f != NULL) {
                        r.identificador = str_dup(nombre); // Duplico porque libero buffer despues
                        r.expresion = f;
                        r.tipo = OP_DEFF;
                    }
                }
            }
        }
    }

    free(buffer);
    return r;
}

static char* parsear_nombre(char** cursor) {
    *cursor += 5;
    avanzar_hasta_noespacio(cursor);
    if (**cursor) {
        // Asumo que un espacio entre el nombre y el '=' debe haber si o si, para diferenciar entre caracteres
        // propios del nombre o no y determinar precisamente cuando termina el identificador.
        char* espacio_despues_de_nombre = strchr(*cursor, ' ');
        if (espacio_despues_de_nombre) {
            *espacio_despues_de_nombre = '\0';
            char* nombre = *cursor;
            if (formato_alfanumerico(nombre)) {
                *cursor = espacio_despues_de_nombre + 1;
                avanzar_hasta_noespacio(cursor);
                if (**cursor && **cursor == '=') {
                    (*cursor)++;
                    avanzar_hasta_noespacio(cursor);
                    return nombre;
                }
            }
        }
    }
    return NULL;
}

static int verificar_funcion(char* funcion_str) {
    char* cursor = funcion_str;
    int invalido = 0, nivel = 0;

    while (*cursor && !invalido) {
        if (identificador_invalido(*cursor) || repeticion_vacia(*cursor, *(cursor + 1))) {
            invalido = 1;
        } else {
            if (*cursor == '<') {
                nivel++;
            } else if (*cursor == '>') {
                nivel--;
                if (nivel < 0)
                    invalido = 1;
            }
            cursor++;
        }
    }

    if (!invalido && nivel == 0) {
        return 0;
    }
    return 1;
}

static int verificar_lista(char* corchete_inicio, char* corchete_final) {
    int invalido = 0;
    char* cursor = corchete_inicio + 1; // Arranco en el primer posible numero
    while (cursor != corchete_final && !invalido) {
        if (!isdigit(*cursor))
            invalido = 1;
        else
            while (isdigit(*cursor)) cursor++;
        if (cursor != corchete_final)  //  Si no era el ultimo digito...
            if (*cursor == ',' ) {
                if (*(cursor + 1) == ' ' && isdigit(*(cursor + 2)))
                    cursor += 2;
                else if (isdigit(*(cursor + 1)))
                    cursor++;
                else
                    invalido = 1;
            } else {
                invalido = 1;
            }
    }
    return invalido;
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

void parser_liberar(ResultadoParser* r) {
    if (!r)
        return;

    if (r->identificador)
        free(r->identificador);

    if (r->tipo == OP_DEFL)
        destruir_lista((Lista*)r->expresion);
    else if (r->tipo == OP_DEFF)
        destruir_funcion((Funcion*)r->expresion);
    r->identificador = NULL;
    r->expresion = NULL;
    r->tipo = OP_INVALIDA;
}
