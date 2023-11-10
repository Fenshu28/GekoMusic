#include <stdio.h>
#include "Listas.h"
#include "Music.h"

void set_Almbum(struct nodo **Lista,char *Alb){
	(*Lista)->info.Album = Alb;
}

char *get_Album(struct nodo *Lista){
	return (*Lista)->info.Album;
}
