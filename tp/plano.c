#include "planos.h"

#include <stdlib.h>
#include <float.h>

#define INFINITO FLT_MAX

struct plano {
    vector_t normal;
    vector_t punto;
};

plano_t *plano_crear(const vector_t normal, const vector_t punto) {
    plano_t *p = malloc(sizeof(plano_t));
    if(p == NULL) {
        return NULL;
    }
    p->normal = normal;
    p->punto = punto;
    return p;
}

void plano_destruir(plano_t *p) {
    free(p);
}

vector_t plano_get_normal(plano_t *p) {
    return p->normal;
}

vector_t plano_get_punto(plano_t *p) {
    return p->punto;
}

float plano_distancia(const plano_t *plano, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal) {
    float p_nd = vector_producto_interno(plano->normal, d);
    if(p_nd == 0) {
        return INFINITO;
    }
    vector_t resta_po = vector_resta(plano->punto, o);
    float t = vector_producto_interno(resta_po, plano->normal) / p_nd;
    if(t < 0) {
        return INFINITO;
    }
    *normal = plano->normal;
    *punto = vector_interpolar_recta(o, d, t);
    return t;
}