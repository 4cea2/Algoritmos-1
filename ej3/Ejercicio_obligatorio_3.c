#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#define INFINITO FLT_MAX
#define PI 3.1415926f
#define ANCHO 640
#define ALTO 480
#define FOV 90
#define VALOR_MAX 255

typedef struct {
    float x, y, z;
} vector_t;

typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    vector_t centro;
    float radio;

    color_t color;

    float ka, kd;
} esfera_t;

typedef struct {
    vector_t posicion;
    color_t color;
    bool es_puntual;
} luz_t;

typedef struct {
    void **v;
    size_t n;
} arreglo_t;

float vector_producto_interno(vector_t a, vector_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector_norma(vector_t a) {
    return sqrt(vector_producto_interno(a, a));
}

vector_t vector_resta(vector_t a, vector_t b) {
    return (vector_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vector_t vector_interpolar_recta(vector_t o, vector_t d, float t) {
    return (vector_t){o.x + d.x * t, o.y + d.y * t, o.z + d.z * t};
}

vector_t vector_normalizar(vector_t a) {
    float norma_a = vector_norma(a);
    return (vector_t){a.x / norma_a, a.y / norma_a, a.z / norma_a};
}

color_t color_sumar(color_t c, color_t m, float p) {
    return (color_t){c.r + m.r * p, c.g + m.g * p, c.b + m.b * p};
}

color_t color_absorber(color_t b, color_t c) {
    return (color_t){b.r * c.r, b.g * c.g, b.b * c.b};
}

int castear_componente(float comp, int x) {
    if(comp > x) {
        comp = x;
    }
    return comp;
}

void color_imprimir(color_t c) {
    int rojo = castear_componente(c.r * VALOR_MAX, VALOR_MAX);
    int verde = castear_componente(c.g * VALOR_MAX, VALOR_MAX);
    int azul = castear_componente(c.b * VALOR_MAX, VALOR_MAX);
    printf("%d %d %d", rojo, verde, azul);
}

esfera_t *esfera_crear(vector_t centro, float radio, color_t color, float ka, float kd) {
    esfera_t * esfera_auxiliar = malloc(sizeof(esfera_t));
    if(esfera_auxiliar == NULL) {
        return NULL;
    }
    esfera_auxiliar->centro = centro;
    esfera_auxiliar->radio = radio;
    esfera_auxiliar->color = color;
    esfera_auxiliar->ka = ka;
    esfera_auxiliar->kd = kd;
    return esfera_auxiliar;
}

void esfera_destruir(esfera_t *esfera) {
    free(esfera);
}

luz_t *luz_crear(vector_t posicion, color_t color, bool es_puntual) {
    luz_t *luz_auxiliar = malloc(sizeof(luz_t));
    if(luz_auxiliar == NULL) {
        return NULL;
    }
    luz_auxiliar->posicion = posicion;
    luz_auxiliar->color = color;
    luz_auxiliar->es_puntual = es_puntual;
    return luz_auxiliar;
}

void luz_destruir(luz_t *luz) {
    free(luz);
}

float esfera_distancia(const esfera_t *esfera, vector_t o, vector_t d, vector_t *punto, vector_t *normal) {
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

bool arreglo_agregar(arreglo_t *a, void *e) {
    void ** auxiliar = realloc(a->v, (a->n + 1) * sizeof(void *));
    if(auxiliar == NULL) {
        return false;
    }
    a->v = auxiliar;
    a->v[a->n++] = e;
    return true;
}

int sombras (const arreglo_t  *esfera, vector_t direccion_luz, vector_t interseccion, size_t esfera_original) {
    vector_t interseccion_luz;
    vector_t normal_luz;
    for(size_t n_esfera = 0; n_esfera < esfera->n; n_esfera++) {
        if(n_esfera == esfera_original) {
            continue;
        }
        if(esfera_distancia(esfera->v[n_esfera], interseccion, direccion_luz, &interseccion_luz, &normal_luz) != INFINITO) {
            return 0;
        }
    }
    return 1;
}

bool intersecta_esfera (const arreglo_t *esferas, vector_t *interseccion, vector_t *normal, vector_t o, vector_t d, size_t *n_esfera) {
    *n_esfera = 0;
    float distancia_menor = INFINITO;
    vector_t aux_inter;
    vector_t aux_normal;
    for(size_t i = 0; i < esferas->n; i++) {
        if(esfera_distancia(esferas->v[i], o, d, interseccion, normal) < distancia_menor) {
            distancia_menor = esfera_distancia(esferas->v[i], o, d, interseccion, normal);
            aux_inter = *interseccion;
            aux_normal = *normal;
            *n_esfera = i;
        }
    }
    if(distancia_menor == INFINITO) {
        return false;
    }
    *interseccion = aux_inter;
    *normal = aux_normal;
    return true;
}

color_t computar_intensidad(const arreglo_t *esferas, const arreglo_t *luces, color_t ambiente, vector_t o, vector_t d) {
    color_t intensidad = {0, 0, 0};
    vector_t normal;
    vector_t interseccion;
    size_t n_esfera;
    if(!intersecta_esfera(esferas, &interseccion, &normal, o, d, &n_esfera)) {
        return intensidad;
    }
    luz_t *luz;
    esfera_t *esfera = esferas->v[n_esfera];
    color_t color = color_sumar((color_t){0, 0, 0}, ambiente, esfera->ka);
    float escalar;
    vector_t luz_direccion;
    for(size_t n_luz = 0; n_luz < luces->n; n_luz++) {
        luz = luces->v[n_luz];
        luz_direccion = luz->posicion;
        if(luz->es_puntual) {
            luz_direccion = vector_normalizar(vector_resta(luz->posicion, interseccion));
        }
        if(vector_producto_interno(luz_direccion, normal) < 0) {
            continue;
        }
        escalar = vector_producto_interno(luz_direccion, normal) * esfera->kd;
        color = color_sumar(color, luz->color, escalar * sombras(esferas, luz_direccion, interseccion, n_esfera));
    }
    intensidad = color_absorber(color, esfera->color);
    return intensidad;
}

int main(void) {
    arreglo_t luces = {NULL, 0};
    arreglo_t esferas = {NULL, 0};

    color_t ambiente = {.05, .05, .05};

    assert(arreglo_agregar(&luces, luz_crear(vector_normalizar((vector_t){0, 1, 0}), (color_t){.2, .2, .2}, false)));
    assert(arreglo_agregar(&luces, luz_crear((vector_t){-2, 10, 2.5}, (color_t){.9, 0, 0}, true)));
    assert(arreglo_agregar(&luces, luz_crear((vector_t){2, 10, 2.5}, (color_t){0, .9, 0}, true)));
    assert(arreglo_agregar(&luces, luz_crear((vector_t){0, 10, 5}, (color_t){0, 0, .9}, true)));

    assert(luces.n == 4);
    for(size_t i = 0; i < luces.n; i++)
        assert(luces.v[i] != NULL);

    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){0, 1, 2.4}, .3, (color_t){1, 1, 1}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){0, -.4, 3}, 1, (color_t){1, 1, 1}, 1, 1)));

    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-2, -.6, 3}, .3, (color_t){1, 0, 0}, 1, .8)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-1.73, -.6, 2}, .3, (color_t){1, 1, 0}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-1, -.6, 1.26}, .3, (color_t){0, 1, 0}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){0, -.6, 1}, .3, (color_t){1, 1, 1}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){1, -.6, 1.26}, .3, (color_t){0, 1, 1}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){1.73, -.6, 2}, .3, (color_t){0, 0, 1}, 1, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){2, -.6, 3}, .3, (color_t){1, 0, 1}, 1, 1)));

    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-3, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, 0)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-2, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, .16)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-1, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, .33)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){0, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, .5)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){1, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, .66)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){2, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, .83)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){3, 2.5, 4.3}, .3, (color_t){1, 1, 1}, 1, 1)));

    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-3, 1.5, 4}, .3, (color_t){1, 1, 1}, 0, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-2, 1.5, 4}, .3, (color_t){1, 1, 1}, .16, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){-1, 1.5, 4}, .3, (color_t){1, 1, 1}, .33, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){0, 1.5, 4}, .3, (color_t){1, 1, 1}, .5, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){1, 1.5, 4}, .3, (color_t){1, 1, 1}, .66, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){2, 1.5, 4}, .3, (color_t){1, 1, 1}, .83, 1)));
    assert(arreglo_agregar(&esferas, esfera_crear((vector_t){3, 1.5, 4}, .3, (color_t){1, 1, 1}, 1, 1)));

    assert(esferas.n == 23);
    for(size_t i = 0; i < esferas.n; i++)
        assert(esferas.v[i] != NULL);


    printf("P3\n");
    printf("%d %d\n", ANCHO, ALTO);
    printf("255\n");

    float vz = ANCHO / 2 / tan(FOV/ 2 * PI / 180);

    for(int vy = ALTO / 2; vy > - ALTO / 2; vy--)
        for(int vx = - ANCHO / 2; vx < ANCHO / 2; vx++) {
            color_imprimir(computar_intensidad(&esferas, &luces, ambiente, (vector_t){0, 0, 0}, vector_normalizar((vector_t){vx, vy, vz})));
            putchar('\n');
        }


    for(size_t i = 0; i < esferas.n; i++)
        esfera_destruir(esferas.v[i]);
    free(esferas.v);

    for(size_t i = 0; i < luces.n; i++)
        luz_destruir(luces.v[i]);
    free(luces.v);

    return 0;
}