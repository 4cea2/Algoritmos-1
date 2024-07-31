#include "esferas.h"

#include <stdlib.h>
#include <float.h>
#include <math.h>

#define INFINITO FLT_MAX

struct esfera {
    vector_t centro;
    float radio;
};

esfera_t *esfera_crear(const vector_t centro, const float radio) {
    esfera_t *esfera = malloc(sizeof(esfera_t));
    if(esfera == NULL) {
        return NULL;
    }
    esfera->centro = centro;
    esfera->radio = radio;
    return esfera;
}

float esfera_distancia(const esfera_t *esfera, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal) {
    vector_t restaco = vector_resta(esfera->centro, o);
    float beta = vector_producto_interno(restaco, d);
    float delta = beta * beta - (vector_producto_interno(restaco, restaco) - (esfera->radio) * (esfera->radio));
    if(delta < 0) {
        return INFINITO; 
    }
    delta = sqrt(delta);
    float t1 = beta + delta;
    float t2 = beta - delta;
    float t = INFINITO;
    if(t2 > 0) {
        t = t2;
    }
    else {
        t = t1;
    }
    if(t < 0) {
        return INFINITO;
    }
    *punto = vector_interpolar_recta(o, d, t);
    vector_t restapc = vector_resta(*punto, esfera->centro);
    *normal = vector_normalizar(restapc);
    return t;
}

vector_t esfera_get_centro(const esfera_t *esfera) {
    return esfera->centro;
}

float esfera_get_radio(const esfera_t *esfera) {
    return esfera->radio;
}

void esfera_destruir(esfera_t *esfera) {
    free(esfera);
}