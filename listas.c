#include "listas.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Ya recibe lista verificada que es minimo [],
 * no importa modificar la original aca, necesito pasarla a lista total
 * despues se pisa el buffer e nuevo.
 */
Lista strlist_to_lista(char* cadena) {
    if (!cadena)
        return NULL;

    size_t len = strlen(cadena);
    Lista lista = lista_crear((FuncionComparadora)cmp_int,
                              (FuncionDestructora)destruir_int,
                              (FuncionVisitante)visitar_int,
                              (FuncionCopia)copiar_int);
    int invalido = 0;
    if (len > 2) {
        // Considero solo lo que esta dentro de los [ ]
        cadena[len-1] = '\0';
        char* cursor = cadena + 1;

        // Aprovecho la poca flexibilidad de
        // separadores (solo ',' y ' ') y así uso strtok
        char* token = strtok(cursor, ", ");
        char* endptr_error;
        while (token != NULL && !invalido) {
            long int num = strtol(token, &endptr_error, 10);
            if (*endptr_error == '\0') {
                int value = (int)num;
                if (value == num)
                    // Verificar que no hubo overflow ya que las
                    // funciones (copy, cmp...) de la lista esperan int's
                    lista_insertar_final(lista, &value);
                else
                    invalido = 1;
            } else {
                invalido = 1;
            }
            token = strtok(NULL, ", ");
        }
    }

    if (invalido)
        return NULL;

    return lista;
}


