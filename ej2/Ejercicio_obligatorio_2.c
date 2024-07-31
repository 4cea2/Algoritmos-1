#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

#define INFINITO FLT_MAX
#define IA 5
#define II 255

#define PI 3.1415926f
#define ANCHO 640
#define ALTO 480
#define FOV 90

const float luz[3] = {0.2873478855663454, 0.9578262852211513, 0 };
const float centros[][3] = {
    {-.4, .75, 1.55},
    {-.55, -.4, 2},
    {3, 1.5, 4},
    {3, 1.5, 5},
    {3, -1.5, 4},
    {3, -1.5, 5},
};
const float radios[] = {
    .3,
    .85,
    .4,
    .4,
    .4,
    .4,
};

float grados_radianes (float grados) {
    return (grados * 2 * PI)/360;
}

float producto_interno(const float a[3], const float b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float norma(const float a[3]) {
    return sqrt(producto_interno(a, a));
}

void normalizar(float a[3]) {
    float norma_a = norma(a);
    a[0] = a[0] / norma_a;
    a[1] = a[1] / norma_a;
    a[2] = a[2] / norma_a;
}

void resta(float r[3], const float a[3], const float b[3]) {
    r[0] = a[0] - b[0];
    r[1] = a[1] - b[1];
    r[2] = a[2] - b[2];
}

void interpolar_recta(float p[3], const float o[3], const float d[3], float t) {
    p[0] = o[0] + t * d[0];
    p[1] = o[1] + t * d[1];
    p[2] = o[2] + t * d[2];
}

float distancia_esfera(const float c[3], float r, const float o[3], const float d[3]) {
    float restaco[3];
    resta(restaco, c, o);
    float beta = producto_interno(restaco, d);
    float delta = beta * beta - (producto_interno(restaco, restaco) - (r * r));
    if ((delta) < 0) {
        return INFINITO;
    }
    delta = sqrt(delta);
    float t1 = beta + delta;
    float t2 = beta - delta;
    if (t2 > 0) {
        return t2;
    }
    if (t1 < 0) {
        return INFINITO; 
    }
    return t1;
}

void normal_esfera(float normal[3], const float c[3], float r, const float p[3]) {
    float punto[3];
    resta(punto, p, c);
    normal [0] = punto [0] / r;
    normal [1] = punto [1] / r;
    normal [2] = punto [2] / r;
}

int sombras (const float cs [][3], const float rs [], size_t n_esferas, size_t esfera_original, const float o[3], const float director_1[3], const float director_2[3]) {
    if(producto_interno (director_1, director_2) < 0) {
        return 0;
    }
    for(size_t n = 0; n < n_esferas; n++) {
        if(n == esfera_original) {
            continue;
        }
        if(distancia_esfera(cs[n], rs[n], o, director_1) != INFINITO) {
            return 0;
        }
    }
    return 1;
}

int computar_intensidad (const float cs[][3], const float rs[], size_t n_esferas, const float luz[3], const float o[3], const float d[3]) {
    for(size_t n = 0; n < n_esferas ; n++) {
        if (distancia_esfera (cs[n], rs[n], o, d) == INFINITO) {
            continue;
        }
        float punto_intersecccion[3];
        float normal[3];
        float t = distancia_esfera (cs[n], rs[n], o, d);
        interpolar_recta(punto_intersecccion, o, d, t);
        normal_esfera (normal, cs[n], rs[n], punto_intersecccion);
        int intensidad = IA + sombras(cs, rs, n_esferas, n, punto_intersecccion, luz, normal) * (II * producto_interno(normal, luz));
        if (intensidad > II) {
            return II;
        }
        return intensidad;
    }
    return 0;
}

void generador_imagen (float x, float y, float z, float o[3]) {
    float director[3];
    size_t n = 6;
    printf ("P2\n%d\t%d\n%d\n", ANCHO, ALTO, II);
    for(float i = (float) ALTO/2; i > y; i--) {
        for(float k = - (float) ANCHO/2; k < x; k++) { 
            director[0] = k;
            director[1] = i;
            director[2] = z;
            normalizar (director);
            printf("%d \n", computar_intensidad(centros, radios, n, luz, o, director));
        }
    }
}

int main (void) {
    float o[3] = {0};
    float x = (float) ANCHO / 2;
    float y = - (float) ALTO / 2;
    float z =  (float) ANCHO / (2 * tan(grados_radianes (FOV) / 2));
    generador_imagen(x, y, z, o);
    return 0;
}