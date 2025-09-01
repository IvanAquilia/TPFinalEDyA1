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
    int en_funcionamiento = 1, guardada;
    Declaraciones declaraciones = declaraciones_crear();
    generar_funciones_base(declaraciones);
    DefParseado* def_parseado;

    while (en_funcionamiento) {
        printf(">>> ");
        fgets(buffer, sizeof(buffer), stdin);
        ResultadoParser r = parser_analizar(buffer);

        switch (r.tipo) {
            case OP_DEFL:
                def_parseado = (DefParseado*)(r.expresion_parseada);
                guardada = definir_lista(def_parseado->identificador, def_parseado->expresion, declaraciones);
                if (!guardada)
                    printf("ERROR: ya existe un elemento con el nombre '%s' en el programa\n", def_parseado->identificador);
                else
                    printf("Lista '%s' definida con exito\n", def_parseado->identificador);
                break;
            case OP_DEFF:
                def_parseado = (DefParseado*)(r.expresion_parseada);
                guardada = definir_funcion(def_parseado->identificador, def_parseado->expresion, declaraciones);
                if (!guardada)
                    printf("ERROR: ya existe un elemento con el nombre '%s' en el programa\n", def_parseado->identificador);
                else
                    printf("Funcion '%s' definida con exito\n", def_parseado->identificador);
                break;
            case OP_APPLY:
                ApplyParseado* apply_parseado = (ApplyParseado*)(r.expresion_parseada);
                Funcion* funcion;
                Lista* lista;
                char *nombre_funcion = apply_parseado->nombre_funcion,
                     *string_lista = apply_parseado->string_lista;
                int in_place = apply_parseado->in_place;
                int obtenidas = obtener_funcion_y_lista(&funcion, &lista,
                                                        apply_parseado->nombre_funcion,
                                                        apply_parseado->string_lista,
                                                        apply_parseado->in_place,
                                                        declaraciones);
                if (!obtenidas) {
                    if (!funcion)
                        printf("ERROR: No existe la funcion de nombre '%s'\n", nombre_funcion);
                    else if (!lista)
                        if (!in_place)
                            printf("ERROR: No existe la lista de nombre '%s'\n", string_lista);
                        else
                            printf("ERROR: Lista mal formada: '%s'\n", string_lista);
                } else {
                    aplicar_funcion(funcion, lista);
                }
                if (lista && in_place)
                    destruir_lista(lista); // Destruyo la lista dummy temporal creada para el in-place apply
                break;
            case OP_SEARCH:
                printf("Buscar transformaciones\n");
                break;
            case OP_INVALIDA:
                printf("ERROR: Instruccion mal formada o no reconocida, intente nuevamente.\n");
                break;
            case OP_EXIT:
                en_funcionamiento = 0;
                break;
        }

        tabla_hash_recorrer(declaraciones);
        parser_liberar(&r); // En el caso de los DEFF y DEFL, una vez que
                            // se transformo la respuesta del
                            // parser a una Declaracion y se guardo en la tabla
                            // mediante un deepcopy, puedo liberar la respuesta.
    }

    tabla_hash_destruir(declaraciones);
    return 0;
}

