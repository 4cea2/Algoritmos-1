#ifndef OBJETO_H
#define OBJETO_H

#include "vector.h"
#include "color.h"

struct objeto;
typedef struct objeto objeto_t;

typedef enum {ESFERA, PLANO, TRIANGULO, MALLA} tipo_t;

//PRE: El color, tipo y el cuerpo tienen que estar inicialidados correctamente.
//POST: Crea al objeto con sus respectivos coeficientes, color, tipo y el cuerpo 
objeto_t *objeto_crear(float ka, float kd, float ks, float kr, color_t color, tipo_t tipo, void *cuerpo);

//PRE(GENERAL): Los objetos de las siguientes 3 funciones tuvieron que haber sido creados de manera correcta.

//POST: Devuelve las caracteristicas del objeto(excepto el cuerpo) por la interfaz.
void objeto_get_comps(objeto_t *o, float *ka, float *kd, float *ks, float *kr, color_t *color, tipo_t *tipo);

//POST: Devuelve el cuerpo del objeto.
void *objeto_cuerpo(objeto_t *objeto);

//PRE: Los vectores tuvieron que haber sido inicializados, en el caso de la normal y el punto no es necesario.
//POST: Devuelve la distancia entre el objeto y la recta con origen o e direccion d, en caso de no haber interseccion, devuelve INFINITO.
float objeto_distancia(const objeto_t *objeto, vector_t o, vector_t d, vector_t *punto, vector_t *normal);

//POST: Libera al objeto.
void objeto_destruir(objeto_t *o);

#endif