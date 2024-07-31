#ifndef ARREGLO_H
#define ARREGLO_H

#include <stdbool.h> 
#include <stddef.h>

struct arreglo;
typedef struct arreglo arreglo_t;

typedef int (*cmp_t)(const void *, const void *);
typedef void (*dty_t)(void *);

//POST: Crea al arreglo.
arreglo_t *arreglo_crear(void);


//POST: Libera el arreglo con sus elementos, en caso de que destruir_elementos no sea NULL, libera los elementos segun el criterio de esta funcion.
void arreglo_destruir(arreglo_t *a, dty_t destruir_elementos);

//PRE(GENERAL): Los arreglos de las siguientes 5 funciones tienen que haber sido creadas.

//POST: Devuelve el tamano del arreglo.
size_t arreglo_get_tamano(const arreglo_t *a);

//POST: Devuelve el elemento en la i-esima posicion del arreglo, en caso de alguna falla, devuelve NULL.
void *arreglo_get_elemento (const arreglo_t *a, size_t i);

//POST: Devuelve true en caso de colocar el elemento e en la posicion i-esima del arreglo, en caso de alguna falla, false.
bool arreglo_set_elemento (arreglo_t *a, size_t i, void *e);

//POST: Devuelve true si se pudo agregar el elemento e al arreglo, en caso de fallas, false.
bool arreglo_agregar (arreglo_t *a, void *e);

//POST: Devuelve true si se pudo encontrar el elemento e en el arreglo segun el criterio de la funcion cmp, si no lo encuentra, false.
bool arreglo_buscar (const arreglo_t *a, void *e, cmp_t cmp);

#endif