#include <stdio.h>
#include <math.h>

#define CX 0.5
#define CY 0.75
#define CZ 1.9
#define R 0.8

#define ANCHO 640
#define ALTO 480
#define FOV 90

#define PI 3.14f

float grados_radianes (float grados){
    return (grados * 2 * PI)/360;
}
 
float producto_interno (float x1, float y1, float z1, float x2, float y2, float z2) {
    return x1 * x2 + y1 * y2 + z1 * z2;
}

float norma (float x, float y, float z) {
    return pow (x * x + y * y + z * z, 0.5);
}

int intersecta_esfera (float dx, float dy, float dz) {
    float a = producto_interno(dx, dy, dz, CX, CY, CZ);
    return (a * a - producto_interno(CX, CY, CZ,CX, CY, CZ) + R * R) >= 0;
}

int main (void) {
    float x = -(float) ANCHO / 2, y = (float) ALTO / 2, z = (float) ANCHO / (2 * tan(grados_radianes (FOV) / 2));
    float dx, dy, dz;
    int vector;
    float norma_d;
    printf ("P1\n%d\t%d\n", ANCHO, ALTO);
    for (float n = -(float) ALTO / 2; y >= n; y--) {
        for (float i = (float) ANCHO / 2; x < i; x++){
            norma_d = norma (x, y, z);
            dx = x / norma_d;
            dy = y / norma_d;
            dz = z / norma_d;
            vector = intersecta_esfera(dx, dy, dz);
            printf ("%d ", vector);
        }
        x -= ANCHO;
    }
    return 0;
}
