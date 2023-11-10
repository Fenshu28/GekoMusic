# La siguiente no es necesariamente requerida, se agrega para
# mostrar como funciona.

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
CFLAGS2 = `pkg-config --libs gtk+-3.0`

SRC = Proyecto.c Listas.c Listas.h
OBJ = Proyecto.o Listas.o


# Reglas explicitas 

all: clean $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) $(CFLAGS2) -o Rep -lvlc
	./Rep
clean:
	$(RM) $(OBJ) Rep

# Reglas implicitas

Cola.o: Listas.c Listas.h
Proyecto.o: Proyecto.c Listas.h
