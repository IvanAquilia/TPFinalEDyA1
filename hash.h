#ifndef HASH_H
#define HASH_H

// ------------------ Funciones genéricas usadas en la interfaz ------------------
typedef unsigned long (*FuncionHash)(const void* dato);
typedef int (*FuncionComparadora)(const void* a, const void* b);
typedef void (*FuncionDestructora)(void* dato);
typedef void* (*FuncionCopia)(const void* dato);
typedef void (*FuncionVisitante)(const void* dato);

// ------------------ Definición de estructuras ------------------
typedef struct {
    void* dato;
    void* clave;
} Entrada;

typedef struct {
    Entrada** buckets;
    unsigned int capacidad;
    unsigned int elementos;

    FuncionHash hash;

    FuncionComparadora cmp;
    FuncionComparadora cmp_clave;

    FuncionDestructora destruir;
    FuncionDestructora destruir_clave;

    FuncionCopia copiar;
    FuncionCopia copiar_clave;

    FuncionVisitante visitar;
} HashTable;

// ------------------ Interfaz pública ------------------

/*
 * Crear tabla hash con funciones auxiliares
 */
HashTable* tabla_hash_crear(int capacidad,
                            FuncionHash hash,
                            FuncionComparadora cmp_clave,
                            FuncionDestructora destruir,
                            FuncionDestructora destruir_clave,
                            FuncionCopia copiar,
                            FuncionCopia copiar_clave,
                            FuncionVisitante visitar);

/*
 * Destruir tabla
 */
void tabla_hash_destruir(HashTable* tabla);

/*
 * Inserta un elemento dado el hasheo obtenido a partir de su clave, no permite claves repetidas.
 * Devuelve 1 si se pudo insertar, de lo contrario 0.
 */
int tabla_hash_insertar(HashTable* tabla, const void* clave, const void* dato);

/*
 * Buscar un elemento dada su clave. Devuelve el puntero guardado "dato" o NULL.
 */
void* tabla_hash_buscar(HashTable* tabla, const void* clave);

/*
 * Recorrer todos los elementos aplicando un visitante a cada "dato" de cada Entrada.
 */
void tabla_hash_recorrer(HashTable* tabla);


#endif
