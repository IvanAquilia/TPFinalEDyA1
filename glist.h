#ifndef GLIST_H
#define GLIST_H

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
    int longitud;
    FuncionComparadora cmp;
    FuncionDestructora destruir;
    FuncionVisitante visitar;
    FuncionCopia copiar;
} GList;

// ------------------ Interfaz pública ------------------

// Crear lista vacía
GList* lista_crear(FuncionComparadora cmp,
                   FuncionDestructora destruir,
                   FuncionVisitante visitar,
                   FuncionCopia copiar);

// Destruir lista (aplica la destructora a cada dato si está definida)
void lista_destruir(GList* lista);

// Insertar al inicio
void lista_insertar_inicio(GList* lista, void* dato);

// Insertar al final
void lista_insertar_final(GList* lista, void* dato);

// Eliminar del inicio (devuelve 0 si está vacía)
int lista_eliminar_inicio(GList* lista);

// Eliminar del final (devuelve 0 si está vacía)
int lista_eliminar_final(GList* lista);

// Obtener primer elemento (o NULL si está vacía)
void* lista_primero(GList* lista);

// Obtener último elemento (o NULL si está vacía)
void* lista_ultimo(GList* lista);

// Buscar un elemento (usa la funcion comparadora)
void* lista_buscar(GList* lista, void* dato);

// Imprimir lista (usa la funcion impresora si está definida)
void lista_imprimir(GList* lista);

// Copiar lista (copia referencias, no clona datos)
GList* lista_copiar(GList* original);

#endif
