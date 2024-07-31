PROGRAM = TP_108304
CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
LFLAGS = -lm

all: $(PROGRAM)

$(PROGRAM): vector.o color.o imagen.o escritura_lectura.o esferas.o plano.o triangulo.o malla.o objeto.o luz.o arreglo.o main.o
	$(CC) $(CFLAGS) vector.o imagen.o color.o escritura_lectura.o esferas.o plano.o triangulo.o malla.o objeto.o luz.o arreglo.o main.o -o $(PROGRAM) $(LFLAGS)

arreglo.o: arreglo.c arreglo.h
	$(CC) $(CFLAGS) -c arreglo.c

luz.o: luz.c luz.h
	$(CC) $(CFLAGS) -c luz.c

objeto.o: objeto.c objeto.h esferas.c triangulo.c plano.c malla.c vector.c
	$(CC) $(CFLAGS) -c objeto.c

malla.o: malla.c malla.h triangulo.c vector.c escritura_lectura.c
	$(CC) $(CFLAGS) -c malla.c

triangulo.o: triangulo.c triangulo.h vector.c
	$(CC) $(CFLAGS) -c triangulo.c

esfera.o: esferas.c esferas.h vector.c
	$(CC) $(CFLAGS) -c esferas.c 

plano.o: plano.c planos.h vector.c
	$(CC) $(CFLAGS) -c plano.c

escritura_lectura.o: escritura_lectura.c imagen.c escritura_lectura_funciones.h color.h
	$(CC) $(CFLAGS) -c escritura_lectura.c

color.o: color.c color_funciones.h
	$(CC) $(CFLAGS) -c color.c

imagen.o: imagen.c color.h
	$(CC) $(CFLAGS) -c imagen.c

vector.o: vector.c vector_funciones.h vector.h
	$(CC) $(CFLAGS) -c vector.c

clean:
	rm *.o
	rm $(PROGRAM)