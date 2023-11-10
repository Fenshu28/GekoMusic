#include <stdio.h>
#include "Listas.h"

typedef struct {
	char *Album;
	char *name;
	char *url;
}Musica;

void set_Almbum(struct nodo **Lista,char *Alb);
char *get_Album(struct nodo *Lista);
