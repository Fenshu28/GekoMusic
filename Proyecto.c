#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <vlc/vlc.h>
#include "Listas.h"

#define sizeIcon 25
#define sizeIconMedia 25
#define MaxList 50
#define MaxCharTex 300

typedef struct{
	char ID[12];
	char Ubicacion[80];
	char Nombre[60];
}ListaReproduccion;

typedef struct{
	libvlc_instance_t *Instancia;
	libvlc_media_player_t *MediaPlayer;
	libvlc_media_t *Media;
}RepMedios;

static void tran_setup(GtkWidget *win);
void Make_UI_Rep();
void Make_UI_Listas();
int LlenarLista(char *folder);
void get_name(char *file,char *dest);
void *Update_Line();

// Variable para las listas de rep
ListaReproduccion ListasSaved[MaxList];
// Variable de lista actual.
struct nodo *ListaRep;
// Variable de Reproductor
RepMedios *Reproductor;
// Widgets y contenedores globales para la interface, que podremos modificar en todo momento
GtkWidget *window,*FrmDialog,*MusicDialog,*boxMain,*ImgAlbum,*boxLef,*boxRig,*boxRigTop,*boxRigBotton,*boxLineTime,*ScrllWn,
			*btnPlay,*btnRandom,*btnRepeat,*btnMusics,*lbNameMusic,*lbNameAutor,*lbNameList,*lbTime,*LineTime;
GtkWidget *dialog,*dialog2; //Widget de ayuda para abrir una ventana donde se elegira la carpeta
// Ajustes de la linea de tiempo de reproducción
GtkAdjustment* ajuste_linea;
// variables contadoras y auxiliares.
int NList,OnPlay = 3;
gboolean OnRepeat,OnRandom,OnList = FALSE,DragLine;
char ActualMusic[300]={"-9999"};
char ActualMusicName[MaxCharTex]={"Canción"};
char ActualAutor[300]={"Autor"};
char ActualList[MaxCharTex]={"Lista actual"};
char ActualListID[12]={"-9999"};
char *NewUbi;
int audio_length;
int ecual = 0;
int64_t current_time;
int64_t milliseconds;
int64_t seconds;
int64_t minutes;
Musica auxMusic;
pthread_t hLineTime;
time_t t;
struct tm *tmp;

/*Función para mostrar una caja de mensaje con un determinado texto.
 * Recibe 1 argumento por referencia: el mensaje a mostrar, 
 * "*aviso" de tipo apuntador a char, que  realmente es un vector 
 * de caracteres. No retorna nada porque la función es vacía.*/
void MsgBox(char *format){
	 FrmDialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"%s",format);
	 gtk_dialog_run(GTK_DIALOG(FrmDialog));
	 gtk_widget_destroy(FrmDialog);
 }
 
 GtkWidget *PushBackground(char *file,int w, int h){
	GtkWidget *picture;
	GdkPixbuf *pixbuf;
	
	/* Cargamos la imagen */
	pixbuf = gdk_pixbuf_new_from_file_at_scale(file, /* nombre de fichero */
							 w, /* ancho de la pantalla */
							 h, /* alto de la pantalla */
							 FALSE, /* No respetar aspecto */
							 NULL);
	/* Guardamos la imagen en un widget */
	picture = gtk_image_new_from_pixbuf(pixbuf);
	//gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(picture));
	return picture;
}

void LoadCSS(){
	GtkCssProvider *cssProvider;
	cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, "./styles.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
											GTK_STYLE_PROVIDER(cssProvider),
											GTK_STYLE_PROVIDER_PRIORITY_USER);
}

static void tran_setup(GtkWidget *win){
  GdkScreen *screen;
  GdkVisual *visual;

  gtk_widget_set_app_paintable(win, TRUE);
  screen = gdk_screen_get_default();
  visual = gdk_screen_get_rgba_visual(screen);

  if (visual != NULL && gdk_screen_is_composited(screen)) {
    gtk_widget_set_visual(win, visual);
  }
}

static void color_back(cairo_t *cr){
  cairo_set_source_rgba(cr, 0.135, 0.206, 0.235, 0.95);
  cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data){
  color_back(cr);
  return FALSE;
}

void Guardar(ListaReproduccion *List){
	FILE *archivo;
	archivo = fopen("Listas.dat","wb");
	if (archivo == NULL)
		exit(1);
	ListaReproduccion *list = List;
	//~ strcpy(user.nombre,name);
	//~ user.codigo = cod;
	//~ user.tiempo = tiem;
	//~ user.puntaje = punt;
	//~ user.nivel = niv;
	//~ user.estrellas = est;
	for(int i = 0; i < NList; i++){
		fwrite(&list[i], sizeof(ListaReproduccion),1,archivo);
	}
	
	
	fclose(archivo);
}

int LoadList(ListaReproduccion *list){
	FILE *archivo;
	int i=0;
	archivo = fopen("Listas.dat","rb");
	
	if(archivo == NULL)
		return -9999;

	do{
		fread(&list[i], sizeof(ListaReproduccion), 1, archivo);
		i++;
	}while(!feof(archivo));
	
	NList = i -1;
	fclose(archivo);
	
	return 0;
}

GtkWidget *ButtonImg(char *label_,char *ImgFile,char *Tooltip,int wImg,int hImg,GCallback handler,char *argm){
	GtkWidget *btn = gtk_button_new_with_label(label_);
	
	if(strcmp(ImgFile,"")){
		GtkWidget *Img = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size(ImgFile,wImg,hImg,NULL));
		gtk_button_set_image(GTK_BUTTON(btn),Img);
		gtk_button_set_image_position(GTK_BUTTON(btn),GTK_POS_TOP);
		gtk_button_set_always_show_image(GTK_BUTTON(btn),TRUE);
	}
	
	if(strcmp(Tooltip,"")){
		gtk_widget_set_tooltip_text(btn,Tooltip);
	}
	
	g_signal_connect(btn,"clicked",handler,argm);
	
	tran_setup(btn);
	
	gtk_style_context_add_class(gtk_widget_get_style_context(btn), "background-trans");
	gtk_style_context_add_class(gtk_widget_get_style_context(btn), "black-bg");
	
	return btn;
}

GtkWidget *MakeLabels(char *Text){
	GtkWidget *w = gtk_label_new(Text);
	gtk_style_context_add_class(gtk_widget_get_style_context(w), "black-bg");
	return w;
}

GtkWidget *MakeSeparator(GtkOrientation orientacion){
	GtkWidget *sep = gtk_separator_new(orientacion);
	gtk_style_context_add_class(gtk_widget_get_style_context(sep), "separator");
	
	return sep;
}

gboolean Buscar_listSaved(const gchar *obj,ListaReproduccion *aux){
	int cent = 0;
	for(int i = 0;i<NList;i++){
		if(!strcmp(obj,ListasSaved[i].ID)){
			*aux = ListasSaved[i];
			return TRUE;
		}
	}
	
	return FALSE;
}

RepMedios *Reproductor_New(char *File){
	RepMedios *NewRep = (RepMedios *) malloc(sizeof(RepMedios));
	
	NewRep->Instancia = libvlc_new (0, NULL);
	NewRep->Media = libvlc_media_new_location(NewRep->Instancia, File);
	NewRep->MediaPlayer = libvlc_media_player_new_from_media(NewRep->Media);
	libvlc_media_player_set_equalizer(NewRep->MediaPlayer,libvlc_audio_equalizer_new_from_preset(ecual));
	/* No need to keep the media now */
     //libvlc_media_release(NewRep->Media);
	
	return NewRep;
}

void isUndknow(char *Cadena,char *Variable){
	if(Cadena == NULL){
		strcpy(Variable,"Desconocido");
	}else{
		get_name(Cadena,Variable);
	}
}

void setDateMusic(){
	isUndknow(libvlc_media_get_meta(Reproductor->Media,libvlc_meta_Title),ActualMusicName);
	isUndknow(libvlc_media_get_meta(Reproductor->Media,libvlc_meta_Artist),ActualAutor);
	
	//libvlc_media_thumbnail_request_by_pos(Reproductor->Instancia,Reproductor->Media,0.0,
											//libvlc_media_thumbnail_seek_precise,100,100,
											//FALSE,libvlc_picture_Png,0);
	
	gtk_label_set_text(GTK_LABEL(lbNameMusic),ActualMusicName);
	gtk_label_set_text(GTK_LABEL(lbNameAutor),ActualAutor);
	gtk_label_set_text(GTK_LABEL(lbNameList),ActualList);
}

void setStop(){
	libvlc_media_player_stop(Reproductor->MediaPlayer);
	
	/* Free the media_player */
	libvlc_media_player_release (Reproductor->MediaPlayer);
	
	//libvlc_release (Reproductor->Instancia);
	if(OnPlay == 2){
		GtkWidget *img = gtk_image_new_from_file("./Icons/play.png");
		gtk_button_set_image(GTK_BUTTON(btnPlay),img);
	}	
	
	OnPlay = 3;
	printf("stop...\n");
}

void setNewPlay(){	
	GtkWidget *img = gtk_image_new_from_file("./Icons/pause.png");
	Reproductor = Reproductor_New(ActualMusic);
	libvlc_media_player_play(Reproductor->MediaPlayer);
	
	//audio_length = (int) libvlc_media_player_get_length(Reproductor->MediaPlayer);
	current_time = 0;
	
	//gtk_button_set_image(GTK_BUTTON(btnPlay),img);
	
	setDateMusic();
	OnPlay = 1;
	
}

void setPlay(){
	libvlc_media_player_play(Reproductor->MediaPlayer);
}

void setPause(){
	libvlc_media_player_pause(Reproductor->MediaPlayer);	
}

void Change_State(){
	if(OnPlay == 1){
		gtk_button_set_image(GTK_BUTTON(btnPlay),gtk_image_new_from_file("./Icons/play.png"));
		OnPlay = 2;
		setPause();
	}else if (OnPlay == 2){
		gtk_button_set_image(GTK_BUTTON(btnPlay),gtk_image_new_from_file("./Icons/pause.png"));
		OnPlay = 1;
		setPlay();
	}else{
		if(strcmp(ActualMusic,"-9999")){
			gtk_button_set_image(GTK_BUTTON(btnPlay),gtk_image_new_from_file("./Icons/pause.png"));
			OnPlay = 1;
			setNewPlay();
		}else{
			MsgBox("No hay una lista de reproducción seleccionada.");
		}
	}
	
	printf("OnPlay: %d\n",OnPlay);
}

void setNext(){
	if(OnPlay != 3){
		if(OnPlay == 2){
			gtk_button_set_image(GTK_BUTTON(btnPlay),gtk_image_new_from_file("./Icons/pause.png"));
		}
		setStop();
		if(OnRandom){
			srand(time(NULL));
			int randm = rand() % Cuenta_nodo(ListaRep) + 1; 
			strcpy(ActualMusic,Buscar_Pos(ListaRep,randm,&auxMusic));
			strcpy(ActualMusicName,auxMusic.name);
			
			setNewPlay();		
		}else {			
			strcpy(ActualMusic,Ir_Siguiente_de(ListaRep,ActualMusic));
			
			Buscar_Elemento(ListaRep,ActualMusic,&auxMusic);
			strcpy(ActualMusicName,auxMusic.name);
			
			setNewPlay();
		}
		setDateMusic();
	}else{
		if(strcmp(ActualMusic,"-9999")){
			strcpy(ActualMusic,Ir_Siguiente_de(ListaRep,ActualMusic));
			setNewPlay();
		}else{
			MsgBox("No hay una lista de reproducción seleccionada");
		}
	}
}

void setPrevious(){
	if(OnPlay != 3){
		if(OnPlay == 2){
			gtk_button_set_image(GTK_BUTTON(btnPlay),gtk_image_new_from_file("./Icons/pause.png"));
		}
		setStop();
		if(OnRandom){
			srand(time(NULL));
			int randm = rand() % Cuenta_nodo(ListaRep) + 1; 
			strcpy(ActualMusic,Buscar_Pos(ListaRep,randm,&auxMusic));
			strcpy(ActualMusicName,auxMusic.name);
						
			setNewPlay();
		}else{
			strcpy(ActualMusic,Ir_Anterior_a(ListaRep,ActualMusic));
			Buscar_Elemento(ListaRep,ActualMusic,&auxMusic);
			strcpy(ActualMusicName,auxMusic.name);
			
			setNewPlay();
		}
		setDateMusic();
	}else{
		if(strcmp(ActualMusic,"-9999")){
			strcpy(ActualMusic,Ir_Anterior_a(ListaRep,ActualMusic));
			setNewPlay();
		}else{
			MsgBox("No hay una lista de reproducción seleccionada");
		}
	}
}

void setRepeat(){
	if(OnRepeat){
		OnRepeat = FALSE;
		gtk_button_set_image(GTK_BUTTON(btnRepeat),gtk_image_new_from_file("./Icons/repeat.png"));
	}else{
		OnRepeat = TRUE;
		gtk_button_set_image(GTK_BUTTON(btnRepeat),gtk_image_new_from_file("./Icons/repeat-true.png"));
	}
}

void setRandom(){
	if(OnRandom){
		OnRandom = FALSE;
		gtk_button_set_image(GTK_BUTTON(btnRandom),gtk_image_new_from_file("./Icons/random.png"));
	}else{
		OnRandom = TRUE;
		gtk_button_set_image(GTK_BUTTON(btnRandom),gtk_image_new_from_file("./Icons/random-true.png"));
	}
}

void *Update_Line(){
	float CurretPorc;
	char Timer[8] = {};
	do{
		sleep(1);
		if(!OnList && OnPlay == 1){
			CurretPorc = libvlc_media_player_get_position(Reproductor->MediaPlayer);
			current_time = libvlc_media_player_get_time(Reproductor->MediaPlayer);//Obtenemos donde se encuentra en este momento
			
			milliseconds = current_time;
			seconds = milliseconds / 1000;
			minutes = seconds / 60;
			milliseconds -= seconds * 1000;
			seconds -= minutes * 60;
			
			sprintf(Timer,"%" PRId64 ":%" PRId64 "-",minutes,seconds);
			gtk_label_set_text(GTK_LABEL(lbTime),Timer);
			
			//printf("Duración: %d\n",((int) libvlc_media_player_get_length(Reproductor->MediaPlayer))/60);
			//printf("Current: %f\n",CurretPorc);
			if(CurretPorc >= 0.99){
				printf("siguiente\n");
				if(OnRepeat){
					setNewPlay();
				}else{
					setNext();
				}				
			}
			if(!DragLine){
				gtk_adjustment_set_value((GtkAdjustment*)ajuste_linea,CurretPorc);
			}
		}
	}while(1);
	
	printf("Saliendo del hilo\n");
	return NULL;
}

void change_Line(GtkWidget *w){
	if(OnPlay != 3 && DragLine){
		libvlc_media_player_set_position(Reproductor->MediaPlayer,gtk_adjustment_get_value((GtkAdjustment*)ajuste_linea));
	}
}

void OnDrag(){
	DragLine = TRUE;
	printf("True\n");
}

void OutDrag(){
	DragLine = FALSE;
	printf("False\n");
	//libvlc_media_player_set_position(Reproductor->MediaPlayer,gtk_adjustment_get_value((GtkAdjustment*)ajuste_linea));
}

void change_List(GtkWidget *btn){
	char NewList[MaxCharTex]={};
	ListaReproduccion aux;
		
	if (OnPlay  == 1 || OnPlay  == 2){
		setStop();
		printf("Stoped...\n");
	}
	
	Buscar_listSaved(gtk_widget_get_name(btn),&aux);
	
	strcpy(ActualList,aux.Nombre);
	strcpy(ActualListID,aux.ID);
	Eliminar_Lista(&ListaRep);	
	LlenarLista(aux.Ubicacion);
	
	strcpy(ActualMusicName,(ListaRep->sig)->info.name);
	strcpy(ActualMusic,(ListaRep->sig)->info.url);

	printf("Musica actual: %s\n",ActualMusic);
	printf("Musica actual 2: %s\n",ActualMusicName);
	
	Change_State();
}

void Make_UI_Listas(){	
	GtkWidget *Grid,*ListDialog,*boxdlg;
	char NamesList[MaxCharTex]={};
		
	// Creamos el cuadro de dialogo de las listas de reproducción.
	ListDialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(ListDialog),"Listas de reproducción creadas.");
	gtk_window_set_default_size(GTK_WINDOW(ListDialog),300,350);
	gtk_window_set_resizable(GTK_WINDOW(ListDialog),FALSE);
	
	// Creamos el contenedor scroll
	Grid = gtk_grid_new();
	ScrllWn = gtk_scrolled_window_new(NULL,NULL);
	
	if(NList == 0){
		GtkWidget *labelUs = gtk_label_new("\n\tNo hay listas de reproducción aún.");
		gtk_grid_attach(GTK_GRID(Grid),labelUs,0,0,1,1);
	}else{
		for(int j = 0;j<NList;j++){
			sprintf(NamesList,"%d.- %s",j+1,ListasSaved[j].Nombre); // Damos formato al nombre de la playlist
			GtkWidget *plist = ButtonImg(NamesList,"","",0,0,G_CALLBACK(change_List),NULL),*box = gtk_box_new(GTK_ORIENTATION_VERTICAL,2); // Creamos los componentes del boton para la playlist
			gtk_widget_set_name(plist,ListasSaved[j].ID);
			gtk_widget_set_size_request(plist,290,10);
			
			if(!strcmp(ListasSaved[j].ID,ActualListID)){
				gtk_style_context_add_class(gtk_widget_get_style_context(plist), "select-color");
			}else{
				gtk_style_context_add_class(gtk_widget_get_style_context(plist), "black-bg");
			}
			
			// Añadimos el boton y su separador en un box que nos ayudara a estilizar mejor la lista de botones.
			gtk_box_pack_start(GTK_BOX(box),plist,FALSE,TRUE,2);
			gtk_box_pack_end(GTK_BOX(box),MakeSeparator(GTK_ORIENTATION_VERTICAL),FALSE,TRUE,2);
			gtk_grid_attach(GTK_GRID(Grid),box,0,j,1,1);
		}
	}
	gtk_widget_set_size_request(ScrllWn,300,350);
	boxdlg = gtk_dialog_get_content_area(GTK_DIALOG(ListDialog));
	
	gtk_container_add(GTK_CONTAINER(ScrllWn),Grid);
	gtk_box_pack_start(GTK_BOX(boxdlg),ScrllWn,FALSE,FALSE,5);
    
    gtk_widget_show_all(boxdlg);
    
    gtk_dialog_run(GTK_DIALOG(ListDialog));
    gtk_widget_destroy(ListDialog);
}

void change_Music(GtkWidget *w){
	setStop();
					
	Buscar_Pos(ListaRep,atoi(gtk_widget_get_name(w)),&auxMusic);
	strcpy(ActualMusic,auxMusic.url);
	strcpy(ActualMusicName,auxMusic.name);
	
	setNewPlay();
	setDateMusic();
	
	gtk_dialog_response(GTK_DIALOG(MusicDialog),0);
}

void Make_UI_Musics(){
	GtkWidget *Grid,*boxdlg;
	char NamesList[5]={};
	Musica aux;
	
	// Creamos el cuadro de dialogo de las listas de reproducción.
	MusicDialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(MusicDialog),ActualList);
	gtk_window_set_default_size(GTK_WINDOW(MusicDialog),600,400);
	gtk_window_set_resizable(GTK_WINDOW(MusicDialog),FALSE);
	
	// Creamos el contenedor scroll
	Grid = gtk_grid_new();
	ScrllWn = gtk_scrolled_window_new(NULL,NULL);
	
	if(NList == 0){
		GtkWidget *labelUs = gtk_label_new("\n\tNo hay una lista de reproducción seleccionada.");
		gtk_grid_attach(GTK_GRID(Grid),labelUs,0,0,1,1);
	}else{
		for(int j = 0;j<Cuenta_nodo(ListaRep);j++){
			Buscar_Pos(ListaRep,j+1,&aux);
			
			GtkWidget *plist = ButtonImg(aux.name,"","",0,0,G_CALLBACK(change_Music),NULL),*box = gtk_box_new(GTK_ORIENTATION_VERTICAL,2); // Creamos los componentes del boton para la playlist
			sprintf(NamesList,"%d",j+1);
			gtk_widget_set_name(plist,NamesList);
			gtk_widget_set_size_request(plist,595,10);
			
			if(!strcmp(aux.url,ActualMusic)){
				gtk_style_context_add_class(gtk_widget_get_style_context(plist), "select-color");
			}else{
				gtk_style_context_add_class(gtk_widget_get_style_context(plist), "black-bg");
			} 
			
			// Añadimos el boton y su separador en un box que nos ayudara a estilizar mejor la lista de botones.
			gtk_box_pack_start(GTK_BOX(box),plist,FALSE,TRUE,2);
			gtk_box_pack_end(GTK_BOX(box),MakeSeparator(GTK_ORIENTATION_VERTICAL),FALSE,TRUE,2);
			gtk_grid_attach(GTK_GRID(Grid),box,0,j,1,1);
		}
	}
	gtk_widget_set_size_request(ScrllWn,600,400);
	boxdlg = gtk_dialog_get_content_area(GTK_DIALOG(MusicDialog));
	
	gtk_container_add(GTK_CONTAINER(ScrllWn),Grid);
	gtk_box_pack_start(GTK_BOX(boxdlg),ScrllWn,FALSE,FALSE,5);
    
    gtk_widget_show_all(boxdlg);
    
    gtk_dialog_run(GTK_DIALOG(MusicDialog));
    gtk_widget_destroy(MusicDialog);
}

void Make_UI_Rep(){
	char imgb[20] = {};
	// Crea la imagen del album por defecto.
	ImgAlbum = gtk_image_new_from_pixbuf(gdk_pixbuf_new_from_file_at_size("./Icons/icono.png",300,300,NULL));
	// Creando los botones pricipales de reproducción y demas componentes
	strcpy(imgb,(OnPlay != 1 ? "./Icons/play.png" : "./Icons/pause.png"));
	btnPlay = ButtonImg("",imgb,"",96,96,G_CALLBACK(Change_State),NULL);
	btnRandom = ButtonImg("","./Icons/random.png","",sizeIconMedia,sizeIconMedia,G_CALLBACK(setRandom),NULL);
	btnRepeat = ButtonImg("","./Icons/repeat.png","",sizeIconMedia,sizeIconMedia,G_CALLBACK(setRepeat),NULL);
	btnMusics = ButtonImg("","./Icons/musics.png","Abre la lista de canciones en tu lista de reproducción.",
							sizeIconMedia,sizeIconMedia,G_CALLBACK(Make_UI_Musics),NULL);
							
	lbNameMusic = MakeLabels(ActualMusicName);
	lbNameAutor = MakeLabels(ActualAutor);
	lbNameList = MakeLabels(ActualList);
	lbTime = MakeLabels("0:0");
	LineTime = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL,ajuste_linea);
	
	gtk_scale_set_value_pos(GTK_SCALE(LineTime),GTK_POS_LEFT);
	gtk_scale_set_draw_value(GTK_SCALE(LineTime),FALSE);
	gtk_scale_set_has_origin(GTK_SCALE(LineTime),FALSE);
	
	//g_signal_connect(G_OBJECT(LineTime),"value_changed",G_CALLBACK(change_Line), NULL);
	//g_signal_connect(G_OBJECT(LineTime),"button-press-event",G_CALLBACK(OnDrag), NULL);
	//g_signal_connect(G_OBJECT(LineTime),"button-release-event",G_CALLBACK(OutDrag), NULL);
	
	// Creando los box contenedores.
	boxRig = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	boxRigTop = gtk_box_new(GTK_ORIENTATION_VERTICAL,2);
	boxRigBotton = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
	boxLineTime = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
	
	// Añadiendo componentes al box derecho.
    gtk_box_pack_start(GTK_BOX(boxRig),boxRigTop,TRUE,TRUE,5);
    gtk_box_pack_start(GTK_BOX(boxRig),MakeSeparator(GTK_ORIENTATION_VERTICAL),FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(boxRig),boxRigBotton,FALSE,FALSE,5);
    
    // Añadiendo componentes al box de linea de tiempo
	gtk_box_pack_start(GTK_BOX(boxLineTime),lbTime,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(boxLineTime),LineTime,TRUE,TRUE,5);
	gtk_box_pack_end(GTK_BOX(boxLineTime),btnMusics,FALSE,FALSE,5);
    
    // Añadiendo componentes al box derecho superior.
    gtk_box_pack_start(GTK_BOX(boxRigTop),lbNameList,TRUE,TRUE,20);
    gtk_box_pack_start(GTK_BOX(boxRigTop),ImgAlbum,TRUE,TRUE,20);
    gtk_box_pack_end(GTK_BOX(boxRigTop),boxLineTime,FALSE,FALSE,5);
    gtk_box_pack_end(GTK_BOX(boxRigTop),lbNameAutor,TRUE,TRUE,5);
    gtk_box_pack_end(GTK_BOX(boxRigTop),lbNameMusic,TRUE,TRUE,5);    
    
    // Añadiendo componentes al box derecho inferior
    gtk_box_pack_start(GTK_BOX(boxRigBotton),btnRandom,FALSE,FALSE,20);
    gtk_box_pack_start (GTK_BOX(boxRigBotton),ButtonImg("","./Icons/previous.png","",sizeIconMedia,sizeIconMedia,G_CALLBACK(setPrevious),NULL),TRUE,TRUE,20);
    gtk_box_pack_start(GTK_BOX(boxRigBotton),btnPlay,FALSE,FALSE,20);
    gtk_box_pack_start(GTK_BOX(boxRigBotton),ButtonImg("","./Icons/next.png","",sizeIconMedia,sizeIconMedia,G_CALLBACK(setNext),NULL),TRUE,TRUE,20);    
    gtk_box_pack_end(GTK_BOX(boxRigBotton),btnRepeat,FALSE,FALSE,20);
    
    // Agregando al box principal.
    gtk_box_pack_start(GTK_BOX(boxMain),boxRig,TRUE,TRUE,5);
    
    // Mostrar todos los componentes
    gtk_widget_show_all(window);
}

gboolean OnFold(char *folder){ // Verifica si el directorio tiene archivos
	char* ext = 0;//el maximo tamaño para el path es de 300
	struct dirent *de;
	DIR *dr = opendir(folder);//Abrimos la ruta
	Musica aux; // Musica auxiliar
	if (dr == NULL){
		return FALSE; // Si no encontramos ningun elemento devolvemos un error
	}
	
	while ((de = readdir(dr)) != NULL){//leemos cada uno de los archivos
		ext = strrchr(de->d_name, '.');
		if(ext!=0){
			if(!strcmp(ext,".mp3") || !strcmp(ext,".mpeg") || !strcmp(ext,".wav") || !strcmp(ext,".ogg") || !strcmp(ext,".m4a") || !strcmp(ext,".opus")){//si sus extensiones son iguales a las soportadas
				return TRUE;
			}
		}
	}
}

void get_name(char *file,char *dest){ // Función que separa el nombre de la extensión
	for(int i = 0;i<strlen(file);i++){
		if(file[i] == 46){
			dest[i] = '\0';
		}else{
			dest[i] = file[i];
		}
	}
}

int LlenarLista(char *folder){ // Función que llena la lista
	char* ext = 0,file[500],name[MaxCharTex];//el maximo tamaño para el path es de 300
	struct dirent *de;
	DIR *dr = opendir(folder);//Abrimos la ruta
	Musica aux; // Musica auxiliar
	
	while ((de = readdir(dr)) != NULL){//leemos cada uno de los archivos
		
		ext = strrchr(de->d_name, '.');
		if(ext!=0){
			if(!strcmp(ext,".mp3") || !strcmp(ext,".mpeg") || !strcmp(ext,".wav") || !strcmp(ext,".ogg") || !strcmp(ext,".m4a") || !strcmp(ext,".opus")){//si sus extensiones son iguales a las soportadas
				strcpy(file,"");//Creamos la ruta
				strcat(file,"file://");
				strcat(file,folder);
				strcat(file,"/");
				strcat(file,de->d_name);
				
				get_name(de->d_name,name);
								
				strcpy(aux.name,name);
				strcpy(aux.url,file);
				
				printf("name: %s\n",aux.name);
				printf("url: %s\n",aux.url);
				
				//añadimos la nueva cancion
				Insertar_Final(&ListaRep,aux);
			}
		}
	}
	
	set_Almbum(&ListaRep,ActualList);
	return 1;
}

gint Make_Name_List(char *name){
	gint resp;
	// Creamos una ventana de dialogo para guardar la lista
	dialog2 = gtk_dialog_new_with_buttons("Guardando playlist",GTK_WINDOW(window),GTK_DIALOG_MODAL, 
											("Cancel"), GTK_RESPONSE_CANCEL, ("Guardar"), GTK_RESPONSE_ACCEPT, NULL);
	gtk_window_set_default_size(GTK_WINDOW(dialog2),250,100);
	gtk_window_set_resizable(GTK_WINDOW(dialog2),FALSE);
	gtk_window_set_decorated(GTK_WINDOW(dialog2),FALSE);
	
	GtkWidget *boxdlg = gtk_dialog_get_content_area(GTK_DIALOG(dialog2)),
				*txtName =gtk_entry_new();
	
	gtk_entry_set_text(GTK_ENTRY(txtName),"Nueva lista de reproducción ");
	//g_signal_connect(txtName,"changed",G_CALLBACK(setDefaultName),NULL);
	
	gtk_box_pack_start(GTK_BOX(boxdlg),gtk_label_new("Nombre de la playlist"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(boxdlg),txtName,FALSE,FALSE,5);
	gtk_widget_show_all(dialog2);
	
	resp = gtk_dialog_run(GTK_DIALOG(dialog2));
	
	strcpy(name,gtk_entry_get_text(GTK_ENTRY(txtName)));
	
	return resp;
}

void OpenList(){ // Función que ayuda a elegir la capeta del album.
	GtkFileChooserAction action =  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	char Name[MaxCharTex];
	dialog = gtk_file_chooser_dialog_new("Abrir carpeta", GTK_WINDOW(window), action, ("Cancel"), GTK_RESPONSE_CANCEL, ("Abrir"), GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),1);
	
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT){ // Si se eligio una carpeta
		// Se recupera la dirección de la caperta elegida
		char *uri = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog))+7;// Obtenemos la direccion es +7 por que "file://" tiene 7 caracteres
		gtk_widget_destroy(dialog);		
		
		if (OnFold(uri)){
			do{				
				gint res = Make_Name_List(Name);
				if(res == GTK_RESPONSE_ACCEPT){
					if(strcmp(Name,"") && strcmp(Name," ")){
						if (OnPlay  == 1 || OnPlay  == 2){
							setStop();
							printf("Stoped...\n");
						}				
						
						Eliminar_Lista(&ListaRep);
						
						t=time(NULL);
						tmp = localtime(&t);
						
						// Agregamos la lista nueva al arreglo de listas.																	
						strcpy( ListasSaved[NList].Nombre,Name);
						strcpy( ListasSaved[NList].Ubicacion,uri);
						strftime(ListasSaved[NList].ID,12,"%d%m%y%H%S",tmp);
						strcpy(ActualList,Name);
						strcpy(ActualListID,ListasSaved[NList].ID);
						printf("ID: %s\n",ListasSaved[NList].ID);
						
						LlenarLista(uri);
						
						strcpy(ActualMusicName,(ListaRep->sig)->info.name);
						strcpy(ActualMusic,(ListaRep->sig)->info.url);
						
						printf("Musica actual: %s\n",ActualMusic);
						printf("Musica actual 2: %s\n",ActualMusicName);
						
						NList ++;
						
						gtk_widget_destroy(dialog2);
						
						Change_State();
						
						Guardar(ListasSaved); // Guarda la lista de reproducción en un archivo Listas .dat
						break;
					}else{
						MsgBox("Debe ingresar un nombre");
						gtk_widget_destroy(dialog2);
					}
				}else{
					printf("Cerrando\n");
					gtk_widget_destroy(dialog2);
					break;
				}
			}while(!strcmp(Name,"") || !strcmp(Name," "));
			
			printf("\nUri: %s\n",uri);
			printf("New Song list: %s\n",Name );
			Imprimir_Lista(ListaRep);// Mostramos la nueva play list
			printf("\nCanciones: %d\n",Cuenta_nodo(ListaRep));
			printf("\n***************************************\n");			
		}else{
			MsgBox("Folder sin canciones\n");
		}		
	}else{
		gtk_widget_destroy(dialog);
	}
	
}

void acerca(){
	GString *lic = g_string_new("");
	GtkWidget *Grid,*aDialog,*boxdlg;
	char NamesList[MaxCharTex]={};
	
	const char *autor[20] = {"Cristopher Alexis Zarate Valencia",
							 "Arleth Machuca Fabian",
							 "Arnold",
							 "Leticia",
							 "Melissa"} ;
							 
	// Creamos el cuadro de dialogo de las listas de reproducción.
	aDialog = gtk_about_dialog_new();
	
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(aDialog),autor);
	printf("paso\n %s",lic->str);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(aDialog),GTK_LICENSE_GPL_3_0);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(aDialog),gdk_pixbuf_new_from_file_at_scale("./Icons/icono.png",80,80,TRUE,NULL));
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(aDialog),"Geko music!");
	gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(aDialog),FALSE);
	
	gtk_dialog_run(GTK_DIALOG(aDialog));
    gtk_widget_destroy(aDialog);
}

/*Función que crea la ventana y da propiedad a sus componentes*/
void activate_win(GtkApplication *app, gpointer user_data){
	// Ajuste de linea de tiempo
	ajuste_linea = gtk_adjustment_new(
		0, // Valor de incio
		0, // Valor minimo
		1, // Valor maximo
		0.1, // Paso de incremento
		0, // NULL
		0 // NULL
	);
	
	// Cargamos el proveedor de estilos CSS.
	LoadCSS();
	
	// Creando los box contenedores.
	boxMain = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,5);
	boxLef = gtk_box_new(GTK_ORIENTATION_VERTICAL,5);
	
	// Añadiendo componentes al box izquierdo.
    gtk_box_pack_start(GTK_BOX(boxLef),ButtonImg("","./Icons/open.png","Crea una lista de reproducción nueva.",sizeIcon,sizeIcon,G_CALLBACK(OpenList),NULL),FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(boxLef),ButtonImg("","./Icons/list.png","Listas de reproducción creadas.",sizeIcon,sizeIcon,G_CALLBACK(Make_UI_Listas),NULL),FALSE,FALSE,5);
    gtk_box_pack_end(GTK_BOX(boxLef),ButtonImg("","./Icons/acerca.png","Acerca de la aplicación.",sizeIcon,sizeIcon,G_CALLBACK(acerca),NULL),FALSE,FALSE,5);
	
	// Agregando al box principal.
    gtk_box_pack_start(GTK_BOX(boxMain),boxLef,FALSE,FALSE,5);
    gtk_box_pack_start(GTK_BOX(boxMain),MakeSeparator(GTK_ORIENTATION_HORIZONTAL),FALSE,FALSE,5);
	
    // Crea la ventana
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window),"Geko Music");
    gtk_window_set_default_size(GTK_WINDOW(window), 1020, 600);
    gtk_widget_set_size_request(window, 1020, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    tran_setup(window);
    // Icono
    gtk_window_set_icon_from_file(GTK_WINDOW(window),"./Icons/icono.png",NULL);
    //gtk_window_set_icon(
        
    // Señales
    g_signal_connect(G_OBJECT(boxMain), "draw",G_CALLBACK(on_draw_event), NULL);
    

    
    // Añade el contenedor fixed a la ventana
    gtk_container_add(GTK_CONTAINER(window), boxMain);
    
    Make_UI_Rep();
    
    OnPlay = 3;
    
    pthread_create(&hLineTime,NULL,&Update_Line,NULL);
    for(int i = 0; i<18;i++){
		printf("Ecual %d: %s\n",i+1,libvlc_audio_equalizer_get_preset_name(i));
	}
	
	LoadList(ListasSaved);
}

int main(int argc, char **argv){
	GtkApplication *app;
    int status;
    ListaRep = Crear_Lista();
    // Inicializa la ventana
    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate_win), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    /* Stop playing */
     libvlc_media_player_stop (Reproductor->MediaPlayer);
 
     /* Free the media_player */
     libvlc_media_player_release(Reproductor->MediaPlayer);
 
     libvlc_release(Reproductor->Instancia);
    
	return 0;
}
