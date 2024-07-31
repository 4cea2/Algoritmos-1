#ifndef ESCRITURA_LECTURA_FUNCIONES_H
#define ESCRITURA_LECTURA_FUNCIONES_H

#include "color.h"
#include "imagen.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************
------------------ESCRITURA-----------------
********************************************/

//-----En las siguientes funciones, el archivo f tiene que estar abierto en escritura-----//

/*
Escribe 2 bytes al archivo f en formato little-endian(binario).
*/
void escribir_int16_little_endian(FILE *f, int16_t v);

/*
Escribe 4 bytes al archivo f en formato little-endian(binario).
*/
void escribir_int32_little_endian(FILE *f, int32_t v);

/*
Escribe 3 bytes al archivo f en formato little-endian(binario).
*/
void escribir_color(int32_t v, FILE *f);

/*
Escribe el color en el formato del archivo f ya sea abierto en modo texto, o binario.
*/
void imprimir_color(color_t c, FILE *f, bool es_texto);

/*
Escribe el archivo f en formato BMP.
*/
void escribir_BMP(const imagen_t *imagen, FILE *f);

/*
Escribe el archivo f en formato PPM.
*/
void escribir_PPM(const imagen_t *imagen, FILE *f);

/*******************************************
------------------LECTURA-----------------
********************************************/

//-----En las siguientes funciones, el archivo f tiene que estar abierto en modo lectura(binario)-----//

/*
Lee 4 bytes del archivo f.
*/
void leer_uint32_little_endian(FILE *f, uint32_t *n);

/*
Lee 3 float del archivo f.
*/
void leer_float_little_endian(FILE *f, float n[]);


#endif