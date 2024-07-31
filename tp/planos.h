#ifndef PLANOS_H
#define PLANOS_H

#include "vector_funciones.h"

struct plano;
typedef struct plano plano_t;

//PRE: El vector normal tiene que tener norma 1 <--> vector normalizado.
//POST: Crea un plano con su respectiva normal y punto. Devuelve NULL en caso de que hubo alguna falla.
plano_t *plano_crear(const vector_t normal, const vector_t punto);

//POST: Libera el plano.
void plano_destruir(plano_t *p);

//PRE(GENERAL): Los planos de las siguientes 3 funciones tuvieron que haber sido creada de manera correcta.

//PRE: Los vectores tuvieron que haber sido inicializados, en el caso de la normal y el punto no es necesario.
//POST: Devuelve la distancia entre el plano y la recta con origen o e direccion d, en caso de no haber interseccion, devuelve INFINITO.
float plano_distancia(const plano_t *plano, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal);

//POST: Devuelve la normal del plano.
vector_t plano_get_normal(plano_t *p);

//POST: Devuelve el punto perteneciente al plano.
vector_t plano_get_punto(plano_t *p);

#endif