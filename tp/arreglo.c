#include "arreglo.h"

#include <stdlib.h>

struct arreglo {
    void **elementos;//Arreglo de tamano i, si i = 0 <---> elementos = NULL.
    size_t i; //Tamano del arreglo.
};

arreglo_t *arreglo_crear(void) {
    arreglo_t *a = malloc(sizeof(arreglo_t));
    if(a == NULL) {
        return NULL;
    }
    a->elementos = NULL;
    a->i = 0;
    return a;
}

void arreglo_destruir(arreglo_t *a, dty_t destruir_elementos) {
    if(destruir_elementos != NULL) {
        for(size_t i = 0; i < a->i; i++) {
            destruir_elementos(a->elementos[i]);
        }
    }
    free(a->elementos);
    free(a);
    return;
}

size_t arreglo_get_tamano(const arreglo_t *a) {
    return a->i;
}

void *arreglo_get_elemento (const arreglo_t *a, size_t i) {
    if(i >= a->i) {
        return NULL;
    }
    return a->elementos[i];
}

bool arreglo_set_elemento (arreglo_t *a, size_t i, void *e) {
    if(i >= a->i) {
        return false;
    }
    a->elementos[i] = e;
    return true;
}

bool arreglo_agregar (arreglo_t *a, void *e) {
    void **aux = realloc(a->elementos, (a->i + 1) * sizeof(void *));
    if(aux == NULL) {
        return false;
    }
    a->elementos = aux;
    a->elementos[a->i] = e;
    a->i++;
    return true;
}

bool arreglo_buscar (const arreglo_t *a, void *e, cmp_t cmp) {
    for(size_t i = 0; i < a->i; i++) {
        if(cmp(a->elementos[i], e) == 0) {
            return true;
        }
    }
    return false;
}
