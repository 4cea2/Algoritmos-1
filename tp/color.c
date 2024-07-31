#include "color_funciones.h"

color_t color_sumar(color_t c, color_t m, float p) {
    return (color_t){c.r + m.r * p, c.g + m.g * p, c.b + m.b * p};
}

color_t color_absorber(color_t b, color_t c) {
    return (color_t){b.r * c.r, b.g * c.g, b.b * c.b};
}