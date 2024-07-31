#ifndef MALLA_H
#define MALLA_H

#include "vector_funciones.h"
#include "triangulo.h"

#include <stdio.h>
#include <stdbool.h> 


struct malla;
typedef struct malla malla_t;

//Crea una malla. En caso de fallar, devuelve NULL.
malla_t *malla_crear(void);

//POST: libera la malla.
void mallas_destruir(malla_t *m);

//PRE(GENERAL): La malla de las siguientes 4 funciones tienen que haber sido creada.

//PRE: El archivo f tiene que estar abierto en modo lectura(binario).
//POST: Inicializa la malla a partir del archivo f.
bool malla_set_comps(malla_t *m, FILE *f);

//PRE: Los vectores tuvieron que haber sido inicializados, en el caso de la normal y el punto no es necesario.
//POST: Devuelve la distancia entre el triangulo de la malla mas cercana a la recta con origen o e direccion d, en caso de no haber interseccion, devuelve INFINITO.
float malla_distancia(const malla_t *malla, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal);

//POST: Devuelve el triangulo i-esimo de la malla, en caso de alguna falla devuelve NULL.
triangulo_t *malla_get_triangulo(malla_t *m, size_t i);

//POST: Devuelve la cantidad de triangulos de la malla.
size_t malla_get_tamano(malla_t *m);

#endif