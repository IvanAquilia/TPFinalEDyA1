#ifndef GLIST_H
#define GLIST_H
#include "utils.h"

// ------------------ Funciones genéricas usadas en la interfaz ------------------
typedef int (*FuncionComparadora)(const void* a, const void* b);
typedef void (*FuncionDestructora)(void* dato);
typedef void* (*FuncionCopia)(const void* dato);
typedef void (*FuncionVisitante)(const void* dato);

// ------------------ Definición de estructuras ------------------
typedef struct _Nodo {
    void* dato;
    struct _Nodo* sig;
    struct _Nodo* ant;
} Nodo;

typedef struct {
    Nodo* head;
    Nodo* tail;
    unsigned int longitud;
    FuncionComparadora cmp;
    FuncionDestructora destruir;
    FuncionVisitante visitar;
    FuncionCopia copiar;
} GList;

// ------------------ Interfaz pública ------------------

/*
 * Crear lista vacía
 */
GList* glist_crear(FuncionComparadora cmp,
                   FuncionDestructora destruir,
                   FuncionVisitante visitar,
                   FuncionCopia copiar);

/*
 * Destruir lista (aplica la destructora a cada dato si está definida)
 */
void glist_destruir(GList* lista);

/*
 * Insertar al inicio
 */
void glist_insertar_inicio(GList* lista, void* dato);

/*
 * Insertar al final
 */
void glist_insertar_final(GList* lista, void* dato);

/*
 * Eliminar del inicio (devuelve 0 si está vacía)
 */
int glist_eliminar_inicio(GList* lista);

/*
 * Eliminar del final (devuelve 0 si está vacía)
 */
int glist_eliminar_final(GList* lista);

/*
 * Obtener primer elemento (o NULL si está vacía)
 */
void* glist_primero(GList* lista);

/*
 * Obtener último elemento (o NULL si está vacía)
 */
void* glist_ultimo(GList* lista);

/*
 * Buscar un elemento (usa la funcion comparadora)
 */
void* glist_buscar(GList* lista, void* dato);

/*
 * Imprimir lista (usa la funcion impresora si está definida)
 */
void glist_imprimir(const GList* lista);

/*
 * Copiar lista (copia referencias, no clona datos)
 */
GList* glist_copiar(const GList* original);

#endif
