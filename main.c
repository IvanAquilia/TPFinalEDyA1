#include "parser.h"
#include "glist.h"
#include "listas.h"
#include "funciones.h"
#include "hash.h"
#include "utils.h"
#include <stdio.h>

int main(void) {
    printf("\n------------INTERPRETE DE LISTAS EDyA1------------\n");
    char buffer[2000];
    int en_funcionamiento = 1;
    Declaraciones declaraciones = declaraciones_crear();
    while (en_funcionamiento) {
        printf(">>> ");
        fgets(buffer, sizeof(buffer), stdin);
        ResultadoParser r = parser_analizar(buffer);

        switch (r.tipo) {
            case OP_DEFL:
                definir_lista(r.identificador, r.expresion, declaraciones);
                break;
            case OP_DEFF:
                definir_funcion(r.identificador, r.expresion, declaraciones);
                break;
            case OP_APPLY:
                printf("Aplicar funcion '%s'\n", r.identificador);
                break;
            case OP_SEARCH:
                printf("Buscar transformaciones\n");
                break;
            case OP_INVALIDA:
                printf("Instruccion mal formada o no reconocida, intente nuevamente.\n");
                break;
            case OP_EXIT:
                en_funcionamiento = 0;
                break;
        }

        tabla_hash_recorrer(declaraciones);
        parser_liberar(&r);
    }

    tabla_hash_destruir(declaraciones);
    return 0;
}

