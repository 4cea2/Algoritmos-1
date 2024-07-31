#include "escritura_lectura_funciones.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define MASK_16_LSB 0x00ff
#define MASK_16_MSB 0xff00
#define MASK_32_A 0x000000ff
#define MASK_32_B 0x0000ff00
#define MASK_32_C 0x00ff0000
#define MASK_32_D 0xff000000
#define DESP_B 8
#define DESP_C 16
#define DESP_D 24

#define VALOR_MAX 255

void escribir_int16_little_endian(FILE *f, int16_t v) {
    uint8_t aux[2] = {(v & MASK_16_LSB), (v & MASK_16_MSB) >> DESP_B};
    assert(fwrite(aux, sizeof(uint8_t), 2, f) == 2);
}

void escribir_int32_little_endian(FILE *f, int32_t v) {
    uint8_t aux[4] = {(v & MASK_32_A), (v & MASK_32_B) >> DESP_B , (v & MASK_32_C) >> DESP_C, (v & MASK_32_D) >> DESP_D};
    assert(fwrite(aux, sizeof(uint8_t), 4, f) == 4);
}

void escribir_color(int32_t v, FILE *f) {
    uint8_t aux[3] = {(v & MASK_32_A), (v & MASK_32_B) >> DESP_B, (v & MASK_32_C) >> DESP_C};
    assert(fwrite(aux, sizeof(uint8_t), 3, f) == 3);
}

static int32_t componer_color(color_t color) {
    uint8_t r = color.r, g = color.g, b = color.b;
    return (uint32_t)((r << DESP_C) | (g << DESP_B) | b);
}

static int castear_componente(float comp, int x) {
    if(comp > x) {
        comp = x;
    }
    return comp;
}

void imprimir_color(color_t c, FILE *f, bool es_texto) {
    int rojo = castear_componente(c.r * VALOR_MAX, VALOR_MAX);
    int verde = castear_componente(c.g * VALOR_MAX, VALOR_MAX);
    int azul = castear_componente(c.b * VALOR_MAX, VALOR_MAX);
    color_t color = {rojo, verde, azul};
    if(es_texto) {
        fprintf(f, "%d %d %d\n", (int)color.r, (int)color.g, (int)color.b);
    }
    else {
        escribir_color(componer_color(color), f);
    }
}

void escribir_BMP(const imagen_t *imagen, FILE *f) {
    size_t ancho;
    size_t alto;
    imagen_dimensiones(imagen, &ancho, &alto);
    size_t multiplo = (ancho % 4);
    uint8_t s[2] = {'B', 'M'};
    assert(fwrite(s, sizeof(uint8_t), 2, f) == 2);
    escribir_int32_little_endian(f, (int32_t)(14 + 40 + alto * (ancho * 3 + multiplo)));
    escribir_int16_little_endian(f, 0x0000);
    escribir_int16_little_endian(f, 0x0000);
    escribir_int32_little_endian(f, (int32_t)54);
    escribir_int32_little_endian(f, (int32_t)40);
    escribir_int32_little_endian(f, (int32_t)ancho);
    escribir_int32_little_endian(f, (int32_t)alto);
    escribir_int16_little_endian(f, (int16_t)1);
    escribir_int16_little_endian(f, (int16_t)24);
    escribir_int32_little_endian(f, 0x00000000);
    escribir_int32_little_endian(f, 0x00000000);
    escribir_int32_little_endian(f, 0x00000000);
    escribir_int32_little_endian(f, 0x00000000);
    escribir_int32_little_endian(f, 0x00000000);
    escribir_int32_little_endian(f, 0x00000000);
    for(size_t y = (alto); y--; ) {
        for(size_t x = 0; x < ancho; x++) {
            imprimir_color(imagen_get_pixel(imagen, x, y), f, false);
        }
        if(multiplo != 0) {
            uint8_t aux[multiplo];
            for(size_t i = 0; i < multiplo; i++) {
                aux[i] = 0x00;
            }
            fwrite(aux, sizeof(uint8_t), multiplo, f);
        }
    }
}

void escribir_PPM(const imagen_t *imagen, FILE *f) {
    size_t ancho;
    size_t alto;
    imagen_dimensiones(imagen, &ancho, &alto);
    fprintf(f, "P3\n%ld\t%ld\n255\n", ancho, alto);
    for(size_t y = 0; y < alto; y++) {
        for(size_t x = 0; x < ancho; x++) {
            imprimir_color(imagen_get_pixel(imagen, x, y), f, true);
        }
    }
}

void leer_uint32_little_endian(FILE *f, uint32_t *n) {
    assert(fread(n, sizeof(uint32_t), 1, f) == 1);
}

void leer_float_little_endian(FILE *f, float n[]) {
    assert(fread(n, sizeof(float), 3, f) == 3);
}