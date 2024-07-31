#ifndef VECTOR_FUNCIONES_H
#define VECTOR_FUNCIONES_H

#include "vector.h"

/*
Devuelve el producto interno de los vectores a y b.
*/
float vector_producto_interno(const vector_t a, const vector_t b);

/*
Devuelve la norma del vector.
*/
float vector_norma(const vector_t a);

/*
Devuelve la resta de los vectores a y b.
*/
vector_t vector_resta(const vector_t a, const vector_t b);

/*
Calcula el punto en donde apunta la recta con origen o en direccion d por un cierto escalar t.
*/
vector_t vector_interpolar_recta(const vector_t o, const vector_t d, const float t);

/*
Devuelve el vector a normalizado.
*/
vector_t vector_normalizar(const vector_t a);

/*
Devuelve el producto vectorial entre los vectores a y b.
*/
vector_t vector_producto_vectorial(const vector_t a, const vector_t b);

#endif