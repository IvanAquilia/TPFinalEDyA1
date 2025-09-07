#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "garray.h"
#include "listas.h"

typedef enum {
    Oi,
    Od,
    Si,
    Sd,
    Di,
    Dd,
    Custom
} TipoFuncion;

typedef struct {
    Lista* lista_resultado;
    int status;
} ResultadoApply;

/*
 * Cada funcion es un array dinamico de los nombres (char*) de las funciones que la compone en orden.
 * Ej. : f1: ["0d", "Mi", "<Dd>", "Si"]
 */
typedef struct {
    GArray* garray;
} Funcion;

typedef struct {
    GArray* garray;
} FuncionesAll;

/*
 */
Funcion* strfunc_to_array(char* nombre, char* cadena, Declaraciones declaraciones);

/*
 */
Funcion* funcion_crear();

/*
 */
FuncionesAll* obtener_todas_funciones_declaradas(Declaraciones declaraciones);

/*
 */
void componer_funcion(Funcion* funcion, char* nombre);

/*
 * Necesitada al destruir una Declaracion
 */
void destruir_funcion(Funcion* funcion);

/*
 */
void destruir_arreglo_aux_funciones(FuncionesAll* funciones_todas);

/*
 * Necesitada al copiar una Declaracion
 */
Funcion* copiar_funcion(const Funcion* funcion);

/*
 * Necesitada al visitar una Declaracion
 */
void visitar_funcion(const Funcion* funcion);

/*
 */
int funciones_iguales(const Funcion* funcion1, const Funcion* funcion2);

/*
 */
TipoFuncion str_a_tipo(char* nombre_funcion);

/*
 */
unsigned int cantidad_composiciones(const Funcion* funcion);

/*
 */
unsigned int cantidad_funciones_totales(const FuncionesAll* funciones);

/*
 */
char* funcion_iesima(const Funcion* funcion, unsigned int i);

/*
 */
Declaracion* funcion_definida_iesima(const FuncionesAll* funciones, unsigned int i);

/*
 */
int definir_funcion(char* nombre, void* funcion, Declaraciones declaraciones);

/*
 */
int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
                            char* nombre_funcion, char* string_lista,
                            int in_place, Declaraciones declaraciones);

/*
 */
ResultadoApply aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones);


#endif
