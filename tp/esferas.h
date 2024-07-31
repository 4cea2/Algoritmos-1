#ifndef ESFERAS_H
#define ESFERAS_H

#include "vector_funciones.h"

#include <stdbool.h>

struct esfera;
typedef struct esfera esfera_t;

//PRE: El vector tiene que estar inicializado.
//POST: Crea la esfera con su respectivo centro y radio, devuelve NULL cuando hubo una falla al crearla.
esfera_t *esfera_crear(const vector_t centro, const float radio);

//POST: Libera la esfera.
void esfera_destruir(esfera_t *esfera);

//PRE(GENERAL): Las esferas de las siguientes 3 funciones tuvieron que haber sido creada de manera correcta.

//PRE: Los vectores tuvieron que haber sido inicializados, en el caso de la normal y el punto no es necesario.
//POST: Devuelve la distancia entre la esfera y la recta con origen o e direccion d, en caso de no haber interseccion, devuelve INFINITO.
float esfera_distancia(const esfera_t *esfera, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal);

//POST: Devuelve el centro de la esfera, si hubo una falla.
vector_t esfera_get_centro(const esfera_t *esfera);

//POST: Devuelve el radio de la esfera.
float esfera_get_radio(const esfera_t *esfera);

#endif