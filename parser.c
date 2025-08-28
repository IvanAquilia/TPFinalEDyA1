#include "parser.h"
#include "utils.h"
#include "listas.h"
#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


ResultadoParser parser_analizar(const char* input) {
    ResultadoParser r = { OP_INVALIDA, NULL, NULL };
    if (!input)
        return r;

    // Lo copio para no modificarlo
    char* buffer = str_dup(input);
    char* cursor = buffer;
    cursor[strcspn(cursor, "\n")] = '\0'; // Tomo hasta '\n'
    cursor = str_trim(cursor);
    size_t len = strlen(cursor);

    // Todas las sentencias válidas deben terminar en ";"
    if (len == 0 || cursor[len-1] != ';') {
        free(cursor);
        return r;
    }

    cursor[len-1] = '\0'; // elimino el '; '
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
                    Lista l = strlist_to_lista(lista);
                    if (l != NULL) {
                        r.nombre = str_dup(nombre);
                        r.expr = l;
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
                Funcion f = strfunc_to_array(cursor);
                if (f != NULL) {
                    r.nombre = str_dup(nombre);
                    r.expr = f;
                    r.tipo = OP_DEFF;
                }
            }
        }
    }

    free(buffer);
    return r;
}

// Obtiene identificador y deja el buffer apuntando al primer elemento del objeto en cuestion a definir
static char* parsear_nombre(char** buffer) {
    *buffer += 5;
    avanzar_hasta_noespacio(buffer);
    if (**buffer) {
        // Asumo que un espacio entre el nombre y el '=' debe haber si o si, para diferenciar entre caracteres
        // propios del nombre o no y determinar precisamente cuando termina el identificador.
        char* espacio_despues_de_nombre = strchr(*buffer, ' ');
        if (espacio_despues_de_nombre) {
            *espacio_despues_de_nombre = '\0';
            char* nombre = *buffer;
            if (formato_alfanumerico(nombre)) {
                *buffer = espacio_despues_de_nombre + 1;
                avanzar_hasta_noespacio(buffer);
                if (**buffer && **buffer == '=') {
                    (*buffer)++;
                    avanzar_hasta_noespacio(buffer);
                    return nombre;
                }
            }
        }
    }
    return NULL;
}

static int verificar_funcion(char* buffer) {
    char* cursor = buffer;
    int cant_abrir_rep = 0, cant_cerrar_rep = 0, invalido = 0;
    while (*cursor && !invalido) {
        if (identificador_invalido(*cursor) || repeticion_vacia(*cursor, *(cursor + 1))) {
            invalido = 1;
        } else {
            if (*cursor == '<')
                cant_abrir_rep++;
            if (*cursor == '>')
                cant_cerrar_rep++;
            cursor++;
        }
    }
    if (!invalido && cant_abrir_rep == cant_cerrar_rep)
        return 0;
    return 1;
}

static int verificar_lista(char* corchete_inicio, char* corchete_final) {
    int invalido = 0;
    char* iterador = corchete_inicio + 1; // Arranco en el primer posible numero
    while (iterador != corchete_final && !invalido) {
        if (!isdigit(*iterador))
            invalido = 1;
        else
            while (isdigit(*iterador)) iterador++;
        if (iterador != corchete_final)  //  Si no era el ultimo digito...
            if (*iterador == ',' ) {
                if (*(iterador + 1) == ' ' && isdigit(*(iterador + 2)))
                    iterador += 2;
                else if (isdigit(*(iterador + 1)))
                    iterador++;
                else
                    invalido = 1;
            } else {
                invalido = 1;
            }
    }
    return invalido;
}

static int formato_alfanumerico(const char* cadena) {
    int formato_correcto = 1;
    while (*cadena && formato_correcto) {
        if (!isalnum(*cadena))
            formato_correcto = 0;
        cadena++;
    }

    return formato_correcto;
}

static int identificador_invalido(char caracter) {
    if (!isalnum(caracter) && caracter != '<' && caracter != '>' && caracter != ' ')
        return 1;
    return 0;
}

static int repeticion_vacia(char caracter_1, char caracter_2) {
    if ((caracter_1 == '<' && caracter_2 == '>') || (caracter_1 == '>' && caracter_2 == '<'))
        return 1;
    return 0;
}

void parser_liberar(ResultadoParser* r) {
    if (!r)
        return;
    free(r->nombre);
    free(r->expr);
    r->nombre = NULL;
    r->expr = NULL;
}
