#ifndef LUZ_H
#define LUZ_H

#include "vector_funciones.h"
#include "color.h"

#include <stdbool.h>

struct luz;
typedef struct luz luz_t;

//PRE: El vector y el booleano deben haber sido inicializado.
//POST: Crea la luz con su respectiva posicion, color y si es puntual o no, en caso de fallar devuelve NULL.
luz_t *luz_crear(vector_t pos, bool es_puntual, color_t color);

//POST: Libera la luz.
void luz_destruir(luz_t *l);

//PRE(GENERAL): La luces de las siguientes 3 funciones tienen que haber sido creadas.

//POST: Devuelve la posicion de la luz.
vector_t luz_get_pos(luz_t *l);

//POST: Devuelve el color de la luz.
color_t luz_get_color(luz_t *l);

//POST: Devuelve true si la luz es puntual, caso contario, false.
bool luz_es_puntual(luz_t *l);

#endif