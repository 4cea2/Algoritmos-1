#include "vector_funciones.h"
#include "color_funciones.h"
#include "escritura_lectura_funciones.h"

#include "planos.h"
#include "esferas.h"
#include "triangulo.h"
#include "malla.h"

#include "luz.h"
#include "objeto.h"
#include "arreglo.h"

#include <stdio.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define INFINITO FLT_MAX
#define PI 3.1415926f
#define ANCHO 640
#define ALTO 480
#define FOV 90
#define MAX_STR 256
#define MAX_COLOR 255


int sombras(const arreglo_t *objetos, const vector_t luz_direccion, const vector_t interseccion, size_t n_objeto) {
    vector_t interseccion_luz;
    vector_t normal_luz;
    size_t tamano = arreglo_get_tamano(objetos);
    for(size_t i = 0; i < tamano; i++) {
        if(i == n_objeto) {
            continue;
        }
        objeto_t *objeto = arreglo_get_elemento(objetos, i);
        if(objeto_distancia(objeto, interseccion, luz_direccion, &interseccion_luz, &normal_luz) != INFINITO) {
            return 0;
        }
    }
    return 1;
}

bool interseccion_objeto(const arreglo_t *objetos, vector_t *interseccion, vector_t *normal, const vector_t o, const vector_t d, size_t *n_objeto) {
    size_t tamano = arreglo_get_tamano(objetos);
    float distancia = INFINITO;
    vector_t aux_inter, aux_normal;
    for(size_t i = 0; i < tamano; i++) {
        objeto_t *objeto = arreglo_get_elemento(objetos, i);
        if(objeto_distancia(objeto, o, d, interseccion, normal) < distancia) {
            distancia = objeto_distancia(objeto, o, d, interseccion, normal);
            aux_inter = *interseccion;
            aux_normal = *normal;
            *n_objeto = i;
        }
    }
    if(distancia == INFINITO) {
        return false;
    }
    *interseccion = aux_inter;
    *normal = aux_normal;
    return true;
}

color_t computar_intensidad(int profundidad, const arreglo_t *objetos, const arreglo_t *luces, color_t ambiente, vector_t o, vector_t d) {
    color_t intensidad = {0, 0, 0};
    vector_t normal, interseccion;
    size_t n_objeto;
    if(!interseccion_objeto(objetos, &interseccion, &normal, o, d, &n_objeto)) {
        return intensidad;
    }
    if(profundidad == 0) {
        return intensidad;
    }
    objeto_t *objeto_cercano = arreglo_get_elemento(objetos, n_objeto);
    float ka, kd, ks, kr;
    color_t color_objeto;
    tipo_t tipo;
    objeto_get_comps(objeto_cercano, &ka, &kd, &ks, &kr, &color_objeto, &tipo);
    luz_t *luz;
    float escalar_1, escalar_2;
    vector_t luz_direccion;
    color_t color = color_sumar((color_t){0, 0, 0}, ambiente, ka);
    vector_t r = vector_normalizar(vector_interpolar_recta(d, normal, (-2) * vector_producto_interno(d, normal)));
    size_t n_luces = arreglo_get_tamano(luces);
    for(size_t n_luz = 0; n_luz < n_luces; n_luz++) {
        luz = arreglo_get_elemento(luces, n_luz);
        luz_direccion = luz_get_pos(luz);
        if(luz_es_puntual(luz)) {
            luz_direccion = vector_normalizar(vector_resta(luz_direccion, interseccion));
        }
        escalar_1 = vector_producto_interno(luz_direccion, normal) * kd;
        escalar_2 = pow(vector_producto_interno(luz_direccion, r), 10) * ks;
        if(vector_producto_interno(luz_direccion, normal) < 0) {
            escalar_1 = 0;
        }
        if(vector_producto_interno(luz_direccion, r) < 0) {
            escalar_2 = 0;
        }
        color = color_sumar(color, luz_get_color(luz), (escalar_1 + escalar_2) * sombras(objetos, luz_direccion, interseccion, n_objeto));
    }
    interseccion = vector_interpolar_recta(interseccion, r, 0.0001);
    profundidad--;
    color = color_sumar(color, computar_intensidad(profundidad, objetos, luces, ambiente, interseccion, r), kr);
    intensidad = color_absorber(color, color_objeto);
    return intensidad;
}

bool leer_formato (const char *s, const char *formato) {
    for(size_t ls = strlen(s) - 1, lfor = strlen(formato); lfor--; ls--) {
        if(formato[lfor] != s[ls]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        fprintf(stderr, "%s [ANCHO] [ALTO] [PROFUNDIDAD] [NOMBRE ARCHIVO]\n", argv[0]);
        return 1;
    }
    char modo[MAX_STR];
    bool formato_ppm = leer_formato(argv[4], ".ppm");
    bool formato_bmp = leer_formato(argv[4], ".bmp");
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
    char *verificacion_profundidad;
    size_t ancho = strtold(argv[1], &verificacion_ancho);
    size_t alto = strtold(argv[2], &verificacion_alto);
    int profundidad = strtold(argv[3], &verificacion_profundidad);
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
    if(verificacion_profundidad != NULL) {
        if(*verificacion_profundidad != '\0') {
            fprintf(stderr, "PROFUNDIDAD no valida\n");
        }
    }
    imagen_t *imagen = imagen_crear(ancho, alto);
    if(imagen == NULL) {
        fprintf(stderr, "No se pudo crear la imagen\n");
        return 1;
    }

    /********************************
    --------------ESCENA-------------
    ********************************/

    arreglo_t *luces = arreglo_crear();
    color_t ambiente = {.05, .05, .05};
    vector_t origen = {0, 0, 0};

    //----------LUCES----------//
    
    assert(arreglo_agregar(luces, luz_crear(vector_normalizar((vector_t){7, 20, -10}), false, (color_t){1, 1, 1})));
    assert(arreglo_agregar(luces, luz_crear(vector_normalizar((vector_t){-7, 20, -10}), false, (color_t){1, 1, 1})));

    assert(arreglo_agregar(luces, luz_crear((vector_t){0, 29, 0}, true, (color_t){(float)129/MAX_COLOR, (float)63/MAX_COLOR, (float)249/MAX_COLOR})));


    
    size_t n_luces = arreglo_get_tamano(luces);
    assert(n_luces == 3);
    for(size_t i = 0; i < n_luces; i++) {
        assert(arreglo_get_elemento(luces, i) != NULL);
    }
    

    arreglo_t *objetos = arreglo_crear();
    
    //----------PLANOS----------//

    //FONDO
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.4, 0.1, 0, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, PLANO, plano_crear((vector_t){0, 0, 1},(vector_t){0, 0, 27}))));
    
    //PISO
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.25, 0.5, 0.1, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, PLANO, plano_crear((vector_t){0, 1, 0},(vector_t){0, -5, 0}))));

    //----------ESFERAS----------//

    //ESFERAS ROJA GRANDE
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.8, 1, 0,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){0, -3.25, 11}, 1.25))));
    
    //ESFERAS VERDE GRANDE
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.8, 1, 0,(color_t){0, 1, 0}, ESFERA, esfera_crear((vector_t){-3.5, -3.25, 11}, 1.25))));
    
    //ESFERAS AZUL GRANDE
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.8, 1, 0,(color_t){0, 0, 1}, ESFERA, esfera_crear((vector_t){3.5, -3.25, 11}, 1.25))));

    //MINIS ESFERAS ROJAS
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){-5.5, -4.80, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){-3.0, -4.80, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){0, -4.80, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){3.0, -4.80, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 0, 0}, ESFERA, esfera_crear((vector_t){5.5, -4.80, 10.10}, 0.25))));
    
    //MINIS ESFERAS BLANCAS
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 1, 1}, ESFERA, esfera_crear((vector_t){-5.5, -4.813, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 1, 1}, ESFERA, esfera_crear((vector_t){-3.0, -4.813, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 1, 1}, ESFERA, esfera_crear((vector_t){0, -4.813, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 1, 1}, ESFERA, esfera_crear((vector_t){3.0, -4.813, 10.10}, 0.25))));
    assert(arreglo_agregar(objetos, objeto_crear(0.5, 0.8, 2, 0.5,(color_t){1, 1, 1}, ESFERA, esfera_crear((vector_t){5.5, -4.813, 10.10}, 0.25))));

    //ESFERAS AMARILLAS
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.9, 2, 0.5,(color_t){1, 1, 0}, ESFERA, esfera_crear((vector_t){-9, 1, 18.75}, 1.10))));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.9, 2, 0.5,(color_t){1, 1, 0}, ESFERA, esfera_crear((vector_t){9, 1, 18.75}, 1.10))));

    //----------TRIANGULOS----------//

    //TRIANGULO FRENTE IZQUIERDA
    vector_t t1_p0 = {-10, 0, 17.9}, t1_p1 = {-8, 0, 17.9}, t1_p2 = {-9, -1, 17.9}, t1_n;
    t1_n = vector_producto_vectorial(vector_resta(t1_p1, t1_p0), vector_resta(t1_p2, t1_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t1_p0, t1_p1, t1_p2, vector_normalizar(t1_n)))));

    //TRIANGULO FRENTE DERECHA
    vector_t t2_p0 = {9, -1, 17.9}, t2_p1 = {8, 0, 17.9}, t2_p2 = {10, 0, 17.9}, t2_n;
    t2_n = vector_producto_vectorial(vector_resta(t2_p1, t2_p0), vector_resta(t2_p2, t2_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t2_p0, t2_p1, t2_p2, vector_normalizar(t2_n)))));

    //TRIANGULO ESPEJO DERECHA
    vector_t t3_p0 = {11.97, 15.5, 27.9}, t3_p1 = {11.97, -2.5, 27.9}, t3_p2 = {11.97, -2, 3}, t3_n;
    t3_n = vector_producto_vectorial(vector_resta(t3_p1, t3_p0), vector_resta(t3_p2, t3_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.6, 1, 0.9, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, TRIANGULO, triangulo_crear(t3_p0, t3_p1, t3_p2, vector_normalizar(t3_n)))));

    //TRIANGULO ESPEJO IZQUIERDA
    vector_t t4_p0 = {-11.97, -2, 3}, t4_p1 = {-11.97, -2.5, 27.9}, t4_p2 = {-11.97, 15.5, 27.9}, t4_n;
    t4_n = vector_producto_vectorial(vector_resta(t4_p1, t4_p0), vector_resta(t4_p2, t4_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.6, 1, 0.9, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, TRIANGULO, triangulo_crear(t4_p0, t4_p1, t4_p2, vector_normalizar(t4_n)))));
    
    //TRIANGULO IZQUIERDA (DE COSTADO).
    vector_t t5_p0 = {-7.9, 0, 17.9}, t5_p1 = {-7.9, 0, 19.9}, t5_p2 = {-7.9, -1, 18.9}, t5_n;
    t5_n = vector_producto_vectorial(vector_resta(t5_p1, t5_p0), vector_resta(t5_p2, t5_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t5_p0, t5_p1, t5_p2, vector_normalizar(t5_n)))));
    
    //TRIANGULO DERECHA (DE COSTADO).
    vector_t t6_p0 = {7.9, -1, 18.9}, t6_p1 = {7.9, 0, 19.9}, t6_p2 = {7.9, 0, 17.9}, t6_n;
    t6_n = vector_producto_vectorial(vector_resta(t6_p1, t6_p0), vector_resta(t6_p2, t6_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t6_p0, t6_p1, t6_p2, vector_normalizar(t6_n)))));
    
    //TRIANGULO IZQUIERDA (FONDO).
    vector_t t7_p0 = {-10.1, 0, 17.9}, t7_p1 = {-10.1, 0, 19.9}, t7_p2 = {-10.1, -1, 18.9}, t7_n;
    t7_n = vector_producto_vectorial(vector_resta(t7_p1, t7_p0), vector_resta(t7_p2, t7_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t7_p0, t7_p1, t7_p2, vector_normalizar(t7_n)))));
    
    //TRIANGULO DERECHA (FONDO).
    vector_t t8_p0 = {10.1, -1, 18.9}, t8_p1 = {10.1, 0, 19.9}, t8_p2 = {10.1, 0, 17.9}, t8_n;
    t8_n = vector_producto_vectorial(vector_resta(t8_p1, t8_p0), vector_resta(t8_p2, t8_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t8_p0, t8_p1, t8_p2, vector_normalizar(t8_n)))));
    
    //TRIANGULO ESPEJO SUPERIOR (FONDO).
    vector_t t9_p0 = {-11.97, 14.8, 26.9}, t9_p1 = {11.97, 14.8, 26.9}, t9_p2 = {11.97, -2, 26.9}, t9_n;
    t9_n = vector_producto_vectorial(vector_resta(t9_p1, t9_p0), vector_resta(t9_p2, t9_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.6, 1, 0.9, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, TRIANGULO, triangulo_crear(t9_p0, t9_p1, t9_p2, vector_normalizar(t9_n)))));

    //TRIANGULO ESPEJO INFERIOR (FONDO).
    vector_t t10_p0 = {-11.97, 14.8, 26.9}, t10_p1 = {11.97, -2, 26.9}, t10_p2 = {-11.97, -2, 26.9}, t10_n;
    t10_n = vector_producto_vectorial(vector_resta(t10_p1, t10_p0), vector_resta(t10_p2, t10_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.6, 1, 0.9, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, TRIANGULO, triangulo_crear(t10_p0, t10_p1, t10_p2, vector_normalizar(t10_n)))));

    //TRIANGULO ATRAS IZQUIERDO
    vector_t t11_p0 = {-8, 0, 20.1}, t11_p1 = {-10, 0, 20.1}, t11_p2 = {-9, -1, 20.1}, t11_n;
    t11_n = vector_producto_vectorial(vector_resta(t11_p1, t11_p0), vector_resta(t11_p2, t11_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t11_p0, t11_p1, t11_p2, vector_normalizar(t11_n)))));

    //TRIANGULO ATRAS DERECHO
    vector_t t12_p0 = {8, 0, 20.1}, t12_p1 = {10, 0, 20.1}, t12_p2 = {9, -1, 20.1}, t12_n;
    t12_n = vector_producto_vectorial(vector_resta(t12_p1, t12_p0), vector_resta(t12_p2, t12_p0));
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.3, 0.5, 0.5, (color_t){1, 1, 0}, TRIANGULO, triangulo_crear(t12_p0, t12_p1, t12_p2, vector_normalizar(t12_n)))));

    //----------MALLAS----------//

    //PIKACHU
    malla_t *pikachu = malla_crear();
    assert(pikachu != NULL);
    FILE *p = fopen("pikachu_final.stl", "rt");
    assert(p != NULL);
    malla_set_comps(pikachu, p);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.5, 0.1, 0.9, (color_t){1, 1, 0}, MALLA, pikachu)));
    assert(fclose(p) != EOF);

    //PARED DERECHA  
    malla_t *pared_derecha = malla_crear();
    assert(pared_derecha != NULL);
    FILE *p_d = fopen("pared_derecha.stl", "rt");
    assert(p_d != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.6, 1, 0, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, pared_derecha)));
    malla_set_comps(pared_derecha, p_d);
    assert(fclose(p_d) != EOF);
    
    //PARED IZQUIERDA
    malla_t *pared_izquierda = malla_crear();
    assert(pared_izquierda != NULL);
    FILE *p_i = fopen("pared_izquierda.stl", "rt");
    assert(p_i != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.9, 1, 0, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, pared_izquierda)));
    malla_set_comps(pared_izquierda, p_i);
    assert(fclose(p_i) != EOF);
    
    //PRIMER ESCALON
    malla_t *primer_escalon = malla_crear();
    assert(primer_escalon != NULL);
    FILE *e_1er = fopen("primer_escalon.stl", "rt");
    assert(e_1er != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.1, 1, 0, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, primer_escalon)));
    malla_set_comps(primer_escalon, e_1er);
    assert(fclose(e_1er) != EOF);

    //SEGUNDO ESCALON
    malla_t *segundo_escalon = malla_crear();
    assert(segundo_escalon != NULL);
    FILE *e_2d0 = fopen("segundo_escalon.stl", "rt");
    assert(e_2d0 != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.1, 1, 0, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, segundo_escalon)));
    malla_set_comps(segundo_escalon, e_2d0);
    assert(fclose(e_2d0) != EOF);
    
    //COLUMNA IZQUIERDA
    malla_t *columna_izquierda = malla_crear();
    assert(columna_izquierda != NULL);
    FILE *c_izq = fopen("columna_izquierda.stl", "rt");
    assert(c_izq != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.1, 0, 0.3, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, columna_izquierda)));
    malla_set_comps(columna_izquierda, c_izq);
    assert(fclose(c_izq) != EOF);

    //COLUMNA DERECHA
    malla_t *columna_derecha = malla_crear();
    assert(columna_derecha != NULL);
    FILE *c_der = fopen("columna_derecha.stl", "rt");
    assert(c_der != NULL);
    assert(arreglo_agregar(objetos, objeto_crear(1, 0.1, 0, 0.3, (color_t){(float)117/MAX_COLOR, (float)123/MAX_COLOR, (float)128/MAX_COLOR}, MALLA, columna_derecha)));
    malla_set_comps(columna_derecha, c_der);
    assert(fclose(c_der) != EOF);
    
/****************************************************/

    size_t n_objetos = arreglo_get_tamano(objetos);
    assert(n_objetos == 36);
    
    for(size_t i = 0; i < n_objetos; i++) {
        assert(arreglo_get_elemento(objetos, i));
    }
    
    float vz = ancho / 2 / tan(FOV/ 2 * PI / 180);
    for(int vy = (float)alto / 2; vy > - (float)alto / 2; vy--) {
        for(int vx = - (float)ancho / 2; vx < (float)ancho / 2; vx++) {
            if(!imagen_set_pixel(imagen, (size_t)(((float)ancho / 2) + vx), (size_t)(((float)alto / 2) - vy), computar_intensidad(profundidad, objetos, luces, ambiente, origen, vector_normalizar((vector_t){vx, vy, vz})))) {
                fprintf(stderr, "Hubo un problema con los pixeles de la imagen\n");
                return 1;
            }
        }
    }
    FILE *archivo = fopen(argv[4], modo);
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
    arreglo_destruir(objetos, (dty_t)objeto_destruir);
    arreglo_destruir(luces, (dty_t)luz_destruir);
    return 0;
}