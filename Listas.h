/*
 * Listas.h
 * Por: Dr. Víctor Alberto Gómez Pérez
 * Universidad de la Sierra Sur
 */

#include <stdio.h>
//#include "Music.h"

typedef struct {
	char album[100];
	char name[200];
	char url[300];
}Musica;

struct nodo{
	Musica info;
	struct nodo *ant;
	struct nodo *sig;
};

// Funciones para listas
struct nodo *crea_nodo();
struct nodo *Crear_Lista();
void Insertar_Principio (struct nodo **Lista, Musica dato);
void Insertar_Final (struct nodo **Lista, Musica dato);
char *Eliminar_Principio(struct nodo **Lista);
char *Eliminar_Final(struct nodo **Lista);
int Buscar_Elemento(struct nodo *Lista,char *nom,Musica	*mus);
char *Buscar_Pos(struct nodo *Lista,int num,Musica *mus);
void Eliminar_Lista(struct nodo **Lista);
void Imprimir_Lista(struct nodo *Lista);
int Cuenta_nodo(struct nodo *Lista);
void Insertar_Antes(struct nodo **Lista, Musica num, char *ele);
void Insertar_Despues(struct nodo **Lista, Musica num, char *ele);
void Eliminar_X(struct nodo **Lista, char *x);
char *Ir_Anterior_a(struct nodo *Lista,char *ele);
char *Ir_Siguiente_de(struct nodo *Lista, char *ele);
// Funciones para Musica
void set_Almbum(struct nodo **Lista,char *Alb);
void get_Album(struct nodo *Lista,char *Alb);
