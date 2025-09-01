#include "parser.h"
#include "listas.h"
#include "funciones.h"
#include "string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Avanza puntero hasta el comienzo de lo que se encuentre luego del signo igual, omitiendo todos los espacios
 * en el medio, luego devuelve dicha string encontrada previa al signo igual, si fue posible.
 */
static char* obtener_cadena_pre_igual(char** cursor);
static char* obtener_nombre_funcion(char** cursor);
static int verificar_funcion(char* funcion_str);
static int identificador_invalido(char caracter);
static int repeticion_vacia(char caracter_1, char caracter_2);
static void terminar_parseo(ResultadoParser* r, void* izq, void* der, TipoOperacion tipo);

ResultadoParser parser_analizar(const char* input) {
    ResultadoParser r;
    r.expresion_parseada = NULL;
    r.tipo = OP_INVALIDA;

    // Lo copio para no modificarlo
    char* buffer = str_dup(input);
    char* cursor = buffer;
    cursor[strcspn(cursor, "\n")] = '\0'; // Tomo hasta primer '\n'
    cursor = str_trim(cursor);
    size_t len = strlen(cursor);

    if (len != 0 && cursor[len-1] == ';') {
        cursor[len-1] = '\0';             // Elimino el '; '

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
                        terminar_parseo(&r, nombre, l, OP_DEFL);
                    }
                }
            }
        } else if (strncmp(cursor, "deff ", 5) == 0){
            char* nombre = obtener_cadena_pre_igual(&cursor);
            if (nombre && *cursor) {
                int valida = verificar_funcion(cursor);
                if (valida) {
                    Funcion* f = strfunc_to_array(cursor);
                    if (f != NULL) {
                        terminar_parseo(&r, nombre, f, OP_DEFF);
                    }
                }
            }
        } else if (strncmp(cursor, "apply ", 6) == 0) {
            char* nombre_funcion = obtener_nombre_funcion(&cursor);
            if (nombre_funcion && *cursor) {
                char* nombre_lista = cursor; // Puede tratarse de una lista in-place
                nombre_lista = str_trim(nombre_lista);
                terminar_parseo(&r, nombre_funcion, nombre_lista, OP_APPLY);
            }
        }
    }

    free(buffer);
    return r;
}

static char* obtener_cadena_pre_igual(char** cursor) {
    *cursor += 5;
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

static int verificar_funcion(char* funcion_str) {
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
            if (!isdigit(*cursor))
                valida = 0;
            else
                while (isdigit(*cursor)) cursor++;
            if (cursor != corchete_final)  //  Si no era el ultimo digito...
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
    return valida;
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

static void terminar_parseo(ResultadoParser* r, void* izq, void* der, TipoOperacion tipo) {
    if (tipo == OP_DEFF || tipo == OP_DEFL) {
        char* nombre = (char*)izq;
        DefParseado* definicion = malloc(sizeof(DefParseado));
        definicion->identificador = str_dup(nombre); // Duplico porque libero buffer despues
        definicion->expresion = der;                 //                       ^
                                                     //                       |
        r->expresion_parseada = (void*)definicion;
        r->tipo = tipo;
    } else if (tipo == OP_APPLY) {
        char* funcion = (char*)izq;
        char* lista = (char*)der;
        ApplyParseado* aplicacion = malloc(sizeof(ApplyParseado));

        aplicacion->nombre_funcion = str_dup(funcion); // Idem ---------------|
        aplicacion->string_lista = str_dup(lista);     // Idem ---------------|

        if (*lista == '[')
            aplicacion->in_place = 1;
        else
            aplicacion->in_place = 0;

        r->expresion_parseada = (void*)aplicacion;
        r->tipo = tipo;
    }
}

void parser_liberar(ResultadoParser* r) {
    if (!r)
        return;

    if (r->expresion_parseada) {
        if (r->tipo == OP_APPLY) {
            ApplyParseado* apply_parseado = (ApplyParseado*)r->expresion_parseada;
            free(apply_parseado->nombre_funcion);
            free(apply_parseado->string_lista);
        } else {
            DefParseado* def_parseado = (DefParseado*)r->expresion_parseada;
            if (def_parseado->identificador)
                free(def_parseado->identificador);

            if (r->tipo == OP_DEFL) {
                destruir_lista((Lista*)def_parseado->expresion);
            } else if (r->tipo == OP_DEFF) {
                destruir_funcion((Funcion*)def_parseado->expresion);
            }
        }
        free(r->expresion_parseada);
    }

    r->tipo = OP_INVALIDA;
}
