#include "luz.h"

#include <stdlib.h>

struct luz {
    vector_t pos;
    bool es_puntual;
    color_t color;
};

luz_t *luz_crear(vector_t pos, bool es_puntual, color_t color) {
    luz_t *l = malloc(sizeof(luz_t));
    if(l == NULL) {
        return NULL;
    }
    l->pos = pos;
    l->es_puntual = es_puntual;
    l->color = color;
    return l;
}

void luz_destruir(luz_t *l) {
    free(l);
}

vector_t luz_get_pos(luz_t *l) {
    return l->pos;
}

color_t luz_get_color(luz_t *l) {
    return l->color;
}

bool luz_es_puntual(luz_t *l) {
    return l->es_puntual;
}

