#include "objeto.h"
#include "color.h"
#include "vector.h"

#include "esferas.h"
#include "planos.h"
#include "triangulo.h"
#include "malla.h"

#include "arreglo.h"

#include <stdlib.h>
#include <float.h>

#define INFINITO FLT_MAX

typedef float (*dist_t)(const void *, const vector_t , const vector_t , vector_t * , vector_t *);

struct objeto {
    float ka, kd, ks, kr;
    color_t color;
    tipo_t tipo;
    void *cuerpo;
};


objeto_t *objeto_crear(float ka, float kd, float ks, float kr, color_t color, tipo_t tipo, void *cuerpo) {
    objeto_t *o = malloc(sizeof(objeto_t));
    if(o == NULL) {
        return NULL;
    }
    o->ka = ka;
    o->kd = kd;
    o->ks = ks;
    o->kr = kr;
    o->color = color;
    o->tipo = tipo;
    o->cuerpo = cuerpo;
    return o;
}

void *objeto_cuerpo(objeto_t *objeto) {
    return objeto->cuerpo;
}

dist_t dist_cuerpo[] = {
    [ESFERA] = (dist_t)esfera_distancia,
    [PLANO] = (dist_t)plano_distancia,
    [TRIANGULO] = (dist_t)triangulo_distancia,
    [MALLA] = (dist_t)malla_distancia,
};

float objeto_distancia(const objeto_t *objeto, vector_t o, vector_t d, vector_t *punto, vector_t *normal) {
    dist_t distancia = dist_cuerpo[objeto->tipo];
    float t = distancia(objeto->cuerpo, o, d, punto, normal);
    if(t == INFINITO) {
        return INFINITO;
    }
    return t;
}

void objeto_get_comps(objeto_t *o, float *ka, float *kd, float *ks, float *kr, color_t *color, tipo_t *tipo) {
    *ka = o->ka;
    *kd = o->kd;
    *ks = o->ks;
    *kr = o->kr;
    *color = o->color;
    *tipo = o->tipo;
}

dty_t dest_cuerpo[] = {
    [ESFERA] = (dty_t)esfera_destruir,
    [PLANO] = (dty_t)plano_destruir,
    [TRIANGULO] = (dty_t)triangulo_destruir,
    [MALLA] = (dty_t)mallas_destruir,
};

void objeto_destruir(objeto_t *o) {
    dty_t destruir_cuerpo = dest_cuerpo[o->tipo];
    destruir_cuerpo(o->cuerpo);
    free(o);
}
