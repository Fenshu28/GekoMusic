/*
 * Listas.c
 * Por: Dr. Víctor Alberto Gómez Pérez
 * Universidad de la Sierra Sur
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Listas.h"
//#include "Music.h"

struct nodo *crea_nodo(){
	return ((struct nodo *)malloc(sizeof(struct nodo)));
}

struct nodo *Crear_Lista(){
	struct nodo *p=crea_nodo();
	strcpy(p->info.name, "-9999");
	strcpy(p->info.url, "-9999");
	p->sig = p;
	p->ant = p;
	return p;
}

void Insertar_Principio (struct nodo **Lista, Musica dato){ // Listo
	struct nodo *aux,*p=crea_nodo();
	p->info = dato;
	
	aux = *Lista;
	
	if((*Lista)->sig==*Lista){
		p->sig=(*Lista)->sig;
		p->ant=aux;
		(*Lista)->sig=p;
		(*Lista)->ant=p;
	}else{
		p->sig=(*Lista)->sig;
		((*Lista)->sig)->ant=p;
		
		p->ant=*Lista;
		(*Lista)->sig=p;
	}	
}

void Insertar_Final (struct nodo **Lista, Musica dato){ // Listo
	struct nodo *p,*aux;
	
	if((*Lista)->sig==*Lista){
           Insertar_Principio(Lista,dato);
	}else{
	     aux=*Lista;
         p=crea_nodo();
         p->info=dato;
         
         p->sig=aux;
         aux->ant = p;
         
         while(aux->sig!=*Lista){
			 aux=aux->sig;
		 }
         
         p->ant = aux;
         aux->sig=p;
	 }
}

char *Eliminar_Principio(struct nodo **Lista){ // Listo
	//printf ("*********\n");
	int num=Cuenta_nodo(*Lista);
	//printf ("------\n");
	char *e;
	struct nodo *p=(*Lista)->sig;
	//printf ("------\n");
	if (num==0){
		printf ("La lista estÃ¡ vacÃ­a...\n");
		return "-9999";
	}else if (num == 1){
			(*Lista)->sig=*Lista;
			(*Lista)->ant=*Lista;
	}else{
			//printf ("1Lista sig: %s\n",((*Lista)->sig)->info.name);
			(*Lista)->sig=p->sig;
			(p->sig)->ant = *Lista;
			//printf ("2Lista sig: %s\n",((*Lista)->sig)->info.name);
		}
		e = p->info.name;
	free (p);
	return e;
}

char *Eliminar_Final(struct nodo **Lista) { // Listo
	int cont;
	char *ele;
	struct nodo *p, *aux,*penul;
	cont=Cuenta_nodo(*Lista);
	aux=*Lista;
	if(cont == 0)
               {
		printf("La lista esta vacia...\n");
                return "-9999";
               }
	else{
		if(cont==1){
			strcpy(ele,(aux->sig)->info.name);
			p = aux->sig;
			(*Lista)->sig=*Lista;
			(*Lista)->ant=*Lista;
		}else{
			do{
				p=aux;
				aux=aux->sig;		
				//printf("p 1: %s\n",p->info.name);
				//printf("p 2: %p\n",p);
			}while(strcmp(aux->info.name,"-9999"));
			
			//printf("\n\naux : %s\n",(*Lista)->info.name);
			//printf("aux p : %p\n----->\n",*Lista);
			ele = p->info.name;
			
			penul=p->ant;
					
			penul->sig = *Lista;
			//printf("p sig : %p\n",penul);
			(*Lista)->ant = penul;
			
			/*printf("---\n");
			printf("p->ant : %s\n",penul->info.name);
			printf("p : %p\n",p);
			printf("aux : %s\n",(*Lista)->info.name);
			printf("ele : %s\n",ele);*/
		}
	}
    free(p);
	return ele;	
}

int Buscar_Elemento(struct nodo *Lista,char *nom,Musica	*mus){ // Listo
	struct nodo *aux;
	int x=1,ban=0;
	aux=Lista->sig;
	if(Lista->sig==Lista)
	   printf("\nLista vací­a Buscar_Elemento\n");
	else{
	     for( ;aux!=Lista && ban==0; ){
			 if(!strcmp(aux->info.url,nom))
				ban=1;
			 aux=aux->sig;
			 x++;
		}
	}
    if(ban==0){
		printf("\nEl elemento dado no existe\n");
		return -9999;
	}else{
		*mus = (aux->ant)->info;
		return x-1;
	}
}

char *Buscar_Pos(struct nodo *Lista,int num,Musica *mus){ // Listo
	struct nodo *aux;
	int x=1,ban=0;
	aux=Lista->sig;
	if(Lista->sig==Lista)
	   printf("\nLista vací­a Buscar_Elemento\n");
	else{
	     for( ;aux!=Lista && ban==0; ){
			 if(x == num){
				ban=1;
				break;
			}
			 aux=aux->sig;
			 x++;
		}
	}
    if(ban==0){
		printf("\nEl elemento dado no existe\n");
		return "-9999";
	}else{
		*mus = aux->info;
		return aux->info.url;		
	}
}

void Eliminar_Lista(struct nodo **Lista){ // Listo
	int i=1,cont;
	struct nodo *aux;
	aux=*Lista;
	cont=Cuenta_nodo(aux);
	//printf("Pass\n");
	//if(cont==0)
		//return;
	//for(i=1;i<cont;i++)
	do{
		Eliminar_Final(Lista);
		i++;
	}while(i < cont);
	
	Eliminar_Principio(Lista);
	printf("\nLista eliminada Eliminar_Lista\n");
}

void Imprimir_Lista(struct nodo *Lista){ // Listo !strcmp(Lista->info.name,"-9999")
	if(Lista->sig==Lista){
		printf("Lista vacía\n");
		printf("Al inicio\n");
		return;
	}else if(Lista == NULL){
		printf("La lista ya no existe\n");
	}
	do{
		Lista = Lista->sig;
		if(strcmp(Lista->info.name,"-9999")){
			printf("->%s\n",Lista->info.name);
		}
	}while(strcmp(Lista->info.name,"-9999"));
	printf("Al inicio\n");
}

int Cuenta_nodo(struct nodo *Lista){ // Listo
	int i=0;
	do{
		
		Lista = Lista->sig;
		if(strcmp(Lista->info.name,"-9999")){
			i++;
			//printf ("%d\n",i);
			//printf ("------\n");
			//sleep(1);
		}
	}while(strcmp(Lista->info.name,"-9999"));
	//printf ("------\n");
	return i;
}

void Insertar_Antes(struct nodo **Lista, Musica num, char *ele){ // Listo
	struct nodo *N, *X, *B;
	int band=1;
	N=*Lista;
	
	while(strcmp(N->info.url,ele) && band == 1){
		if(N->sig!=*Lista){
			B = N;
			N = N->sig;
		}else
			band = 0;
	}
	if(band==1){
		X=crea_nodo();
		X->info=num;
		if((*Lista)->sig == N)
			Insertar_Principio(Lista, num);
		else{
			B->sig=X;
			X->ant=B;
			X->sig =N;
			N->ant = X;
		}
	}
	else
		printf("El nodo %s dado como referencia no se encuentra en la lista.\n",ele);
}

void Insertar_Despues(struct nodo **Lista, Musica num, char *ele){ // Listo
	struct nodo *B, *N, *X;
	int band =1;
	B=*Lista;
	while(strcmp(B->info.url,ele) && band==1){
		if(B->sig!=*Lista){
			B=B->sig;
			N=B->sig;
		}else
			band = 0;
	}
	if(band==1){
		if(N != *Lista){
			X=crea_nodo();
			X->info=num;
			
			X->sig=N;
			N->ant = X;
			
			B->sig=X;
			X->ant=B;
		}else{
			Insertar_Final(Lista,num);			
		}		
	}else
		printf("El nodo %s dado como referencia no se encuentra en la lista.\n",ele);
}

void Eliminar_X(struct nodo **Lista, char *x){
	struct nodo *Q, *T;
	int band = 1;
	Q=*Lista;
	while(strcmp(Q->info.url,x) && band == 1){
		if(Q->sig!=*Lista){
			T=Q;
			Q=Q->sig;
		}else
			band = 0;
	}
	if(band==0){
		printf("El nodo con informacion %s no se encuentra en la lista.\n",x);
	}else{
		if((*Lista)->sig == Q)
			Eliminar_Principio(Lista);
		else
			T->sig=Q->sig;
			(Q->sig)->ant=T;
			
            free(Q);
	}
	
}

char *Ir_Anterior_a(struct nodo *Lista,char *ele){
		struct nodo *N, *X, *B;
	int band=1;
	N=Lista;
	
	while(strcmp(N->info.url,ele) && band == 1){
		if(N->sig!=Lista){
			B = N;
			N = N->sig;
		}
		else
			band = 0;
	}
	if(band==1){
		if(B==Lista){
			return (B->ant)->info.url;
		}else{
			return B->info.url;
		}		
	}
	else
		printf("El nodo %s dado como referencia no se encuentra en la lista.\n",ele);
	return "-9999";	
}

char *Ir_Siguiente_de(struct nodo *Lista, char *ele){ // Listo
	struct nodo *B, *N, *X;
	int band =1;
	B=Lista;
	
	while(strcmp(B->info.url,ele) && band==1){
		if(B->sig!=Lista){
			B=B->sig;
			N=B->sig;
		}else
			band = 0;
	}
	if(band==1){
		if(N == Lista){
			return (N->sig)->info.url;
		}else{
			return N->info.url;
		}		
	}else
		printf("El nodo %s dado como referencia no se encuentra en la lista.\n",ele);	
	return "-9999";
}

void set_Almbum(struct nodo **Lista,char *Alb){
	strcpy((*Lista)->info.album,Alb);
}

void get_Album(struct nodo *Lista,char *Alb){
	strcpy(Alb,Lista->info.album);
}












