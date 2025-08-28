#include "parser.h"
#include "glist.h"
#include "listas.h"
#include <stdio.h>

#include "funciones.h"

int main() {
    printf("\n------------INTERPRETE DE LISTAS EDyA1------------\n");
    char buffer[256];
    int en_funcionamiento = 1;

    while (en_funcionamiento) {
        printf(">>> ");
        fgets(buffer, sizeof(buffer), stdin);
        ResultadoParser r = parser_analizar(buffer);

        switch (r.tipo) {
            case OP_DEFL:
                lista_imprimir((Lista)r.expr);
                printf("\n");
                break;
            case OP_DEFF:
                funcion_mostrar((Funcion)r.expr);
                printf("\n");
                break;
            case OP_APPLY:
                printf("Aplicar funcion '%s'\n", r.nombre);
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

        parser_liberar(&r);
    }

    return 0;
}
