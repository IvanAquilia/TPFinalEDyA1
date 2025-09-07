#include "parser.h"
#include "listas.h"
#include "funciones.h"
#include "hash.h"
#include "utils.h"
#include "search.h"
#include <stdio.h>


int main(void) {
    Declaraciones declaraciones = declaraciones_crear();
    if (!generar_funciones_base(declaraciones)) {
        printf("ERROR: error fatal al intentar inicializar el programa.\n");
        return 1;
    }

    printf("\n------------INTERPRETE DE LISTAS EDyA1------------\n");
    char buffer[2000];
    int en_funcionamiento = 1, guardada;

    while (en_funcionamiento) {
        printf(">>> ");
        fgets(buffer, sizeof(buffer), stdin);
        ResultadoParser r = parser_analizar(buffer, declaraciones);

        switch (r.tipo) {
            case OP_DEFL:
                char* id_lista = r.parte_izquierda;
                void* def_lista = r.parte_derecha;
                guardada = definir(LISTA, id_lista, def_lista, declaraciones);
                if (guardada)
                    printf("Lista '%s' definida con exito\n", id_lista);
                else
                    printf("ERROR: ya existe un elemento con el nombre '%s' en el programa\n", id_lista);

                break;
            case OP_DEFF:
                char* id_funcion = r.parte_izquierda;
                void* def_funcion = r.parte_derecha;
                guardada = definir(FUNCION, id_funcion, def_funcion, declaraciones);
                if (guardada)
                    printf("Funcion '%s' definida con exito\n", id_funcion);
                else
                    printf("ERROR: ya existe un elemento con el nombre '%s' en el programa\n", id_funcion);

                break;
            case OP_APPLY:
                char* nombre_funcion = r.parte_izquierda;
                char* string_lista = (char*)r.parte_derecha;
                int in_place = r.in_place;
                Funcion* funcion = NULL;
                Lista* lista = NULL;

                int obtenidas = obtener_funcion_y_lista(&funcion, &lista, nombre_funcion,
                    string_lista, in_place, declaraciones);
                if (obtenidas) {
                    ResultadoApply resultado = aplicar_funcion(funcion, lista, declaraciones);
                    if (resultado.status == 0)
                        visitar_lista(resultado.lista_resultado);
                    else if (resultado.status < 0)
                        printf("ERROR: se ha alcanzado el limite de memoria/tiempo para realizar el calculo.\n");
                    else if (resultado.status > 0)
                        printf("ERROR: operacion ilegal.\n");

                    destruir_lista(resultado.lista_resultado);
                } else {
                    if (!funcion)
                        printf("ERROR: no existe la funcion de nombre '%s'\n", nombre_funcion);
                    if (!lista) {
                        if (!in_place)
                            printf("ERROR: no existe la lista de nombre '%s'\n", string_lista);
                        else
                            printf("ERROR: lista literal mal formada: '%s'\n", string_lista);
                    }
                }
                if (lista && in_place)
                    destruir_lista(lista); // Destruyo la lista dummy temporal creada para el in-place apply

                break;
            case OP_SEARCH:
                void* def_search = r.parte_derecha;
                printf("Buscando...\n");
                Funcion* resultado_search = search(declaraciones, def_search);
                if (resultado_search != NULL) {
                    printf("Funcion encontrada: \n");
                    visitar_funcion(resultado_search);
                    printf("\n");

                    destruir_funcion(resultado_search);
                }
                else
                    printf("No se encontro una funcion que cumpla con el objetivo en el tiempo provisto. \n");

                break;
            case OP_INVALIDA:
                printf("ERROR: Instruccion mal formada o no reconocida, intente nuevamente.\n");
                break;
            case OVERFLOW_LISTA:
                printf("ERROR: la lista contiene naturales muy grandes que exceden el limite de procesamiento.\n");
                break;
            case FUNCION_INEXISTENTE:
                printf("ERROR: solo se pueden componer funciones previamente existentes en el programa.\n");
                break;
            case SEARCH_INVALIDO:
                printf("ERROR: las sentencias de search solo pueden poseer pares de listas existentes en el programa.\n");
                break;
            case OP_EXIT:
                en_funcionamiento = 0;
                break;
        }
        parser_liberar(&r);
    }

    destruir_declaraciones(declaraciones);
    return 0;
}

