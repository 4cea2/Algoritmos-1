#include "triangulo.h"

#include <stdlib.h>
#include <float.h>

#define INFINITO FLT_MAX

struct triangulo {
    vector_t p0, p1, p2;
    vector_t normal;
};

triangulo_t *triangulo_crear(const vector_t p0, const vector_t p1, const vector_t p2, const vector_t normal) {
    triangulo_t *t = malloc(sizeof(triangulo_t));
    if(t == NULL) {
        return NULL;
    }
    t->normal = normal;
    t->p0 = p0;
    t->p1 = p1;
    t->p2 = p2;
    return t;
}

void triangulo_destruir(triangulo_t *t) {
    free(t);
}

float triangulo_distancia(const triangulo_t *triangulo, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal) {
    float epsilon = 0.0000001;
    vector_t e_t1 = vector_resta(triangulo->p1, triangulo->p0);
    vector_t e_t2 = vector_resta(triangulo->p2, triangulo->p0);
    vector_t h = vector_producto_vectorial(d, e_t2);
    float a = vector_producto_interno(e_t1, h);
    if((a > -epsilon) && (a < epsilon)) {
        return INFINITO;
    }
    vector_t s = vector_resta(o, triangulo->p0);
    float u = vector_producto_interno(s, h) / a;
    if((u < 0) || (u >1)) {
        return INFINITO;
    }
    vector_t q = vector_producto_vectorial(s, e_t1);
    float v = vector_producto_interno(d, q) / a;
    if((v < 0) || (u + v > 1)) {
        return INFINITO;
    }
    float t = vector_producto_interno(e_t2, q) / a;
    if(t < 0) {
        return INFINITO;
    }
    *normal = triangulo->normal;
    *punto = vector_interpolar_recta(o, d, t);
    return t;
}

vector_t triangulo_get_p0(const triangulo_t *t) {
    return t->p0;
}

vector_t triangulo_get_p1(const triangulo_t *t) {
    return t->p1;
}

vector_t triangulo_get_p2(const triangulo_t *t) {
    return t->p2;
}

vector_t triangulo_get_normal(const triangulo_t *t) {
    return t->normal;
}

