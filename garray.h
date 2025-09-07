#ifndef GARRAY_H
#define GARRAY_H
#include "utils.h"

// ------------------ Funciones genéricas usadas en la interfaz ------------------
typedef int (*FuncionComparadora)(const void* a, const void* b);
typedef void (*FuncionDestructora)(void* dato);
typedef void* (*FuncionCopia)(const void* dato);
typedef void (*FuncionVisitante)(const void* dato);

// ------------------ Definición de estructuras ------------------
typedef struct {
    void** elementos;
    unsigned int capacidad;
    unsigned int tamaño_actual;
    FuncionComparadora cmp;
    FuncionDestructora destruir;
    FuncionVisitante visitar;
    FuncionCopia copiar;
} GArray;

// ------------------ Interfaz pública ------------------

/*
 * Crear array vacío
 */
GArray* garray_crear(unsigned int capacidad_inicial,
                    FuncionComparadora cmp,
                    FuncionDestructora destruir,
                    FuncionVisitante visitar,
                    FuncionCopia copiar);

/*
 * Destruir array (aplica la destructora a cada dato)
 */
void garray_destruir(GArray* array);

/*
 * Insertar al final
 */
void garray_insertar(GArray* array, void* dato);

/*
 * Eliminar del final (devuelve 0 si está vacío)
 */
int garray_eliminar(GArray* array);

/*
 * Obtener elemento en posición específica
 */
void* garray_obtener(GArray* array, unsigned int posicion);

/*
 * Imprimir array (usa la funcion visitante)
 */
void garray_imprimir(const GArray* array);

/*
 * Copiar array (deep-copy)
 */
GArray* garray_copiar(const GArray* original);

#endif
