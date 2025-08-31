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
} Entrada;

typedef struct {
    Entrada** buckets;
    unsigned int capacidad;
    unsigned int elementos;

    FuncionHash hash;
    FuncionComparadora cmp;
    FuncionDestructora destruir;
    FuncionCopia copiar;
    FuncionVisitante visitar;
} HashTable;

// ------------------ Interfaz pública ------------------

/*
 * Crear tabla hash con funciones auxiliares
 */
HashTable* tabla_hash_crear(int capacidad,
                            FuncionHash hash,
                            FuncionComparadora cmp,
                            FuncionDestructora destruir,
                            FuncionCopia copiar,
                            FuncionVisitante visitar);

/*
 * Destruir tabla
 */
void tabla_hash_destruir(HashTable* tabla);

/*
 * Inserta un elemento, no permite repetidos. Devuelve 1 si se pudo insertar, de lo contrario 0.
 */
int tabla_hash_insertar(HashTable* tabla, const void* dato);

/*
 * Buscar un elemento. Devuelve el puntero guardado o NULL.
 */
void* tabla_hash_buscar(HashTable* tabla, const void* dato);

/*
 * Recorrer todos los elementos aplicando un visitante
 */
void tabla_hash_recorrer(HashTable* tabla);


#endif
