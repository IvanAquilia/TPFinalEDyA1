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


/*
 * Alias para el GArray que contiene todas las declaraciones de funciones del sistema
 */
typedef struct {
    GArray* garray;
} FuncionesAll;

/*
 * Ya recibe string verificada, es decir, se sabe que es una seguidilla de alfanumericos + espacios
 * sabiendo tambien que hay la misma cantidad de '<' que de '>',
 * Lee la string en dicho formato y por cada nombre de funcion que encuentre, intenta buscarla en el sistema
 * y crear una nueva funcion que sea el resultado de la composicion de las mismas.
 * Modifica la cadena original, no hay problema, es el ultimo eslabón del parser.
 */
Funcion* strfunc_to_funcion(char* cadena, Declaraciones declaraciones);

/*
 * Crea una funcion, con su GArray correspondiente.
 */
Funcion* funcion_crear();

/*
 * Crea un GArray con todas las declaraciones de funciones del sistema.
 * Itera por toda la tabla de Declaraciones, por cada vez que encuentre un elemento que
 * sea de tipo Funcion, lo inserta en el GArray.
 */
FuncionesAll* obtener_todas_funciones_declaradas(Declaraciones declaraciones);

/*
 * Agrega un identificador de funcion existente al final de la definición de otra funcion.
 */
void componer_funcion(Funcion* funcion, char* nombre);

/*
 * Destruye una funcion.
 */
void destruir_funcion(Funcion* funcion);

/*
 * Destruye el GArray que contiene todas las declaraciones de funciones del sistema.
 */
void destruir_arreglo_aux_funciones(FuncionesAll* funciones_todas);

/*
 * Copia una función.
 */
Funcion* copiar_funcion(const Funcion* funcion);

/*
 * Visita una funcion, depende de la funcion visitante pasada al GArray, pero generalmente se usará para imprimir.
 */
void visitar_funcion(const Funcion* funcion);

/*
 * Determina si 2 funciones son la misma mediante sus composiciones.
 */
int funciones_iguales(const Funcion* funcion1, const Funcion* funcion2);

/*
 * Dado el identificador de una función, determina si se trata de una funcion base
 * (y de cuál) o de una definida por el usuario. Retorna un alias amigable para comparacion.
 */
TipoFuncion str_a_tipo(char* nombre_funcion);

/*
 * Retorna la cantidad de composiciones de la que está hecha esta función.
 */
unsigned int cantidad_composiciones(const Funcion* funcion);

/*
 * Dado el GArray de todas las funciones del sistmea.
 * Retorna la cantidad de declaraciones de funciones totales en el sistema.
 */
unsigned int cantidad_funciones_totales(const FuncionesAll* funciones);

/*
 * Dada una funcion, devuelve el identificador de la funcion compuesta
 * ubicada en la posición i de izquierda a derecha.
 */
char* funcion_iesima(const Funcion* funcion, unsigned int i);

/*
 * Dado el listado de todas las funciones definidas del sistema, devuelve la definicion
 * ubicada en la posicion i de izquierda a derecha. Util para recorrer el listado sin necesidad
 * de acceder a la interfaz de GArray.
 */
Declaracion* funcion_definida_iesima(const FuncionesAll* funciones, unsigned int i);

/*
 * Dados el nombre de una funcion, y el nombre o definicion (indicado por el argumento in_place) de una lista
 * guarda en los punteros de argumento correspondientes los objetos encontrados (o creados temporalmente en el
 * caso de la lista in_place) en la tabla de Declaraciones, en caso de no poder concretar la accion, devuelve 0, sino 1.
 */
int obtener_funcion_y_lista(Funcion** funcion, Lista** lista,
    char* nombre_funcion, char* string_lista,
    int in_place, Declaraciones declaraciones);

/*
 * Entrada publica al sistema de aplicacion, copia la lista, inicializa contadores y llama a aplicar_interno.
 * Devuelve en una estructura el status de la aplicacion y la lista copia potencialmente modificada.
 * A cargo del caller destruir la copia que se devuelve.
 */
ResultadoApply aplicar_funcion(Funcion* funcion, Lista* lista, Declaraciones declaraciones);


#endif
