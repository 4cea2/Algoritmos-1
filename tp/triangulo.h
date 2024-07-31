#ifndef TRIANGULO_H
#define TRIANGULO_H

#include "vector_funciones.h"

struct triangulo;
typedef struct triangulo triangulo_t;

//PRE: Los vectores p0, p1, p2 tienen que estar inicializados, la normal tiene que estar normalizada.
//POST: Crea el triangulo con sus respectivos puntos y su una normal originada por las mismas.
triangulo_t *triangulo_crear(const vector_t p0, const vector_t p1, const vector_t p2, const vector_t normal);

//POST: Libera el triangulo.
void triangulo_destruir(triangulo_t *t);

//PRE(GENERAL): Los triangulos de las siguientes 5 funciones tuvieron que haber sido creado correctamente.

//PRE: Los vectores o y d tienen que estar inicializados, en el caso del punto y la normal no es necesario.
//POST: Devuelve la distancia entre el triangulo y la recta con origen o e direccion d, en caso de no haber interseccion, devuelve INFINITO.
float triangulo_distancia(const triangulo_t *triangulo, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal);

//POST: Devuelve el punto p0 del trianulo.
vector_t triangulo_get_p0(const triangulo_t *t);

//POST: Devuelve el punto p1 del triangulo.
vector_t triangulo_get_p1(const triangulo_t *t);

//POST: Devuelve el punto p2 del triangulo.
vector_t triangulo_get_p2(const triangulo_t *t);

//POST: Devuelve la normal del triangulo.
vector_t triangulo_get_normal(const triangulo_t *t);

#endif