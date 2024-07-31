#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "imagen.h"
#include "color.h"

#define FOV 90
#define PI 3.1415926f
#define MAX_ITER 80

#define MASK_16_LSB 0x00ff
#define MASK_16_MSB 0xff00
#define MASK_32_A 0x000000ff
#define MASK_32_B 0x0000ff00
#define MASK_32_C 0x00ff0000
#define MASK_32_D 0xff000000
#define DESP_B 8
#define DESP_C 16
#define DESP_D 24

#define MAX_STR 256
#define VALOR_MAX 255

typedef struct {
    float x, y, z;
} vector_t;

float vector_producto_interno(vector_t a, vector_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector_norma(vector_t a) {
    return sqrt(vector_producto_interno(a, a));
}

vector_t vector_interpolar_recta(vector_t o, vector_t d, float t) {
    return (vector_t){o.x + d.x * t, o.y + d.y * t, o.z + d.z * t};
}

vector_t vector_normalizar(vector_t a) {
    float norma_a = vector_norma(a);
    return (vector_t){a.x / norma_a, a.y / norma_a, a.z / norma_a};
}

color_t computar_intensidad(color_t ambiente, vector_t o, vector_t d) {
    vector_t c = (vector_t){(d.x / d.z) * 1.5 - .5, (d.y / d.z) * 1.5 + .1, 0};
    vector_t z = {0, 0, 0};
    int n = 0;
    while(vector_norma(z) <= 2 && n < MAX_ITER) {
        z = (vector_t){z.x * z.x - z.y * z.y, z.x * z.y + z.x * z.y, 0};
        z = vector_interpolar_recta(z, c, 1);
        n++;
    }

    if(n == MAX_ITER)
        return ambiente;

    float h = (n + 1 - log(log2(vector_norma(z)))) / MAX_ITER * 360;
    float x = (1 - fabs(fmodf(h / 60.0, 2) - 1));
    float r = 0, g = 0, b = 0;

    if(h < 60)
        r = 1, g = x;
    else if(h < 120)
        g = 1, r = x;
    else if(h < 180)
        g = 1, b = x;
    else if(h < 240)
        b = 1, g = x;
    else if(h < 300)
        b = 1, r = x;
    else
        r = 1, b = x;

    return (color_t){r, g, b};
}

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

int32_t componer_color(color_t color) {
    uint8_t r = color.r, g = color.g, b = color.b;
    return (uint32_t)((r << DESP_C) | (g << DESP_B) | b);
}

int castear_componente(float comp, int x) {
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
            assert(fwrite(aux, sizeof(uint8_t), multiplo, f) == multiplo);
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

bool leer_formato (const char *s, const char *formato) {
    for(size_t ls = strlen(s) - 1, lfor = strlen(formato); lfor--; ls--) {
        if(formato[lfor] != s[ls]) {
            return false;
        }
    }
    return true;
}

int main (int argc, char *argv[]) {
    if(argc != 4) {
        fprintf(stderr, "%s [ANCHO] [ALTO] [NOMBRE ARCHIVO]\n", argv[0]);
        return 1;
    }
    char modo[MAX_STR];
    bool formato_ppm = leer_formato(argv[3], ".ppm");
    bool formato_bmp = leer_formato(argv[3], ".bmp");
    if(formato_ppm) {
        strcpy(modo, "wt");
    }
    else if(formato_bmp) {
        strcpy(modo, "wb");
    }
    else {
        fprintf(stderr, "Extension no valida, debe ser .ppm o .bmp\n");
        return 1;
    }
    char *verificacion_ancho;
    char *verificacion_alto;
    size_t ancho = strtold(argv[1], &verificacion_ancho);
    size_t alto = strtold(argv[2], &verificacion_alto);
    if(verificacion_ancho != NULL) {
        if(*verificacion_ancho != '\0') {
            fprintf(stderr, "ANCHO no valido\n");
            return 1;
        }
    }
    if(verificacion_alto != NULL) {
        if(*verificacion_alto != '\0') {
            fprintf(stderr, "ALTO no valido\n");
            return 1;
        }
    }
    imagen_t *imagen = imagen_crear(ancho, alto);
    if(imagen == NULL) {
        fprintf(stderr, "No se pudo crear la imagen\n");
        return 1;
    }
    color_t ambiente = {.05, .05, .05};
    vector_t origen = {0, 0, 0};
    float vz = ancho / 2 / tan(FOV/ 2 * PI / 180);
    for(int vy = (float)alto / 2; vy > - (float)alto / 2; vy--) {
        for(int vx = - (float)ancho / 2; vx < (float)ancho / 2; vx++) {
            if(!imagen_set_pixel(imagen, (size_t)(((float)ancho / 2) + vx), (size_t)(((float)alto / 2) - vy), computar_intensidad(ambiente, origen, vector_normalizar((vector_t){vx, vy, vz})))) {
                fprintf(stderr, "Hubo un problema con los pixeles de la imagen\n");
                return 1;
            }
        }
    }
    FILE *archivo = fopen(argv[3], modo);
    if(archivo == NULL) {
        fprintf(stderr, "No se pudo abrir el archivo\n");
        return 1;
    }
    if(formato_ppm) {
        escribir_PPM(imagen, archivo);
    }
    else {
        escribir_BMP(imagen, archivo);
    }
    if(fclose(archivo) == EOF) {
        fprintf(stderr, "Hubo un error en algun momento, el archivo puede estar corrupto\n");
        return 1;
    }
    imagen_destruir(imagen);
    return 0;
}