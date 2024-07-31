#ifndef IMAGEN_H
#define IMAGEN_H

#include "color.h"

#include <stddef.h>
#include <stdbool.h>


struct imagen;
typedef struct imagen imagen_t;

//PRE: El ancho y el alto tienen que estar inicializados.
//POST: Crea la imagen, en caso de fallar, devuelve NULL.
imagen_t *imagen_crear(size_t ancho, size_t alto);

//POST: Libera la imagen.
void imagen_destruir(imagen_t *im);

//PRE(GENERAL): Las imagenes de las siguientes 3 funciones tienen que estar creadas.

//POST: Devuelve el ancho y el alto por interfaz(pueden estar inicializados o no).
void imagen_dimensiones(const imagen_t *im, size_t *ancho, size_t *alto);

//POST: Devuelve true si se pude agregar el color a la imagen en la posicion x e y, si falla devuelve false.
bool imagen_set_pixel(imagen_t *im, size_t x, size_t y, color_t color);

//POST: Devuelve el color de la imagen que esta en la posicion x e y.
color_t imagen_get_pixel(const imagen_t *im, size_t x, size_t y);

#endif