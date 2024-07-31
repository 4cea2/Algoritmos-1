#include "arreglo.h"
#include "vector_funciones.h"
#include "escritura_lectura_funciones.h"
#include "triangulo.h"
#include "malla.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>

#define INFINITO FLT_MAX

struct malla{
    arreglo_t *triangulos;
};

malla_t *malla_crear(void) {
    malla_t *m = malloc(sizeof(malla_t));
    if(m == NULL) {
        return NULL;
    }
    arreglo_t *t = arreglo_crear();
    if(t == NULL) {
        free(m);
        return NULL;
    }
    m->triangulos = t;
    return m;
}

static void leer_descarte(FILE *f) {
    uint64_t descarte;
    for(size_t i = 0; i < 10; i++) {
        assert(fread(&descarte, sizeof(uint64_t), 1, f) == 1);
    }
}

static void vector_set(vector_t *v, float a[]) {
    v->x = a[0];
    v->y = a[1];
    v->z = a[2];
}

bool malla_set_comps(malla_t *m, FILE *f) {
    leer_descarte(f);
    uint32_t cantidad;
    uint16_t descarte_16bits;
    leer_uint32_little_endian(f, &cantidad);
    size_t n_normales = cantidad;
    vector_t n0, p0, p1, p2;    
    float aux[3];
    for(size_t i = 0; i < n_normales; i++) {
        leer_float_little_endian(f, aux);
        vector_set(&n0, aux);
        leer_float_little_endian(f, aux);
        vector_set(&p0, aux);
        leer_float_little_endian(f, aux);
        vector_set(&p1, aux);
        leer_float_little_endian(f, aux);
        vector_set(&p2, aux);
        assert(fread(&descarte_16bits, sizeof(uint16_t), 1, f) == 1);
        triangulo_t *t = triangulo_crear(p0, p1, p2, n0);
        if(t == NULL) {
            return false;
        }
        if(!arreglo_agregar(m->triangulos, t)) {
            return false;
        }
    }
    return true;
}


float malla_distancia(const malla_t *malla, const vector_t o, const vector_t d, vector_t *punto, vector_t *normal) {
    size_t n = arreglo_get_tamano(malla->triangulos);
    float f = INFINITO;
    float aux;
    size_t i_triangulo;
    for(size_t i = 0; i < n; i++) {
        aux = triangulo_distancia(arreglo_get_elemento(malla->triangulos, i), o, d, punto, normal);
        if(f > aux) {
            f = aux;
            i_triangulo = i;
        }
    }
    if(f == INFINITO) {
        return f;
    }
    triangulo_t *triangulo_cercano = arreglo_get_elemento(malla->triangulos, i_triangulo);
    assert(triangulo_cercano != NULL);
    vector_t nor = triangulo_get_normal(triangulo_cercano);
    *normal = nor;
    *punto = vector_interpolar_recta(o, d, f);
    return f;
}

triangulo_t *malla_get_triangulo(malla_t *m, size_t i) {
    triangulo_t *aux = arreglo_get_elemento(m->triangulos, i);
    if(aux == NULL) {
        return NULL;
    }
    return aux;
}

size_t malla_get_tamano(malla_t *m) {
    return arreglo_get_tamano(m->triangulos);
}

void mallas_destruir(malla_t *m) {
    arreglo_destruir(m->triangulos, (dty_t)triangulo_destruir);
    free(m);
}