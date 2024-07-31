#include "vector_funciones.h"

#include <math.h>

float vector_producto_interno(const vector_t a, const vector_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector_norma(const vector_t a) {
    return pow(vector_producto_interno(a, a), 0.5);
}

vector_t vector_resta(const vector_t a, const vector_t b) {
    vector_t v;
    v.x = a.x - b.x;
    v.y = a.y - b.y;
    v.z = a.z - b.z;
    return v;
}

vector_t vector_interpolar_recta(const vector_t o, const vector_t d, const float t) {
    vector_t v;
    v.x = o.x + d.x * t;
    v.y = o.y + d.y * t;
    v.z = o.z + d.z * t;
    return v;
}

vector_t vector_normalizar(const vector_t a) {
    vector_t v;
    float norma = vector_norma(a);
    v.x = a.x / norma;
    v.y = a.y / norma;
    v.z = a.z / norma;
    return v;
}

vector_t vector_producto_vectorial(const vector_t a, const vector_t b) {
    vector_t v;
    v.x = a.y * b.z - b.y * a.z;
    v.y = -(a.x * b.z - b.x * a.z);
    v.z = a.x * b.y - b.x * a.y;
    return v;
}
