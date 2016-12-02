#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN
void map_new (unsigned width, unsigned height)
{
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (6);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  //Fleurs
  map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);
  //Pieces
  map_object_add ("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);
  

  map_object_end ();

}

void map_save (char *filename)
{
  int valeur, err,tmp;
  int height = map_height(); 
  int width = map_width();
  int frame, type1,type2,type3,type4;
  char name[100];
  int nb_object = map_objects();
  //------------ouverture du fichier de sauvgarde---------------
  int output = open("maps/saved.map", O_TRUNC | O_WRONLY | O_CREAT,0666);
  if(output == -1){
    fprintf (stderr,"Problème dans maps/saved.map: %s\n",filename);
    exit(1);
  }
  lseek(output, 0, SEEK_SET);
  //-----------------sauvgarde de la taille---------------------
  write(output,&width,sizeof(int));
  write(output,&height,sizeof(int));
  //------------------sauvgarde des objets----------------------
  write(output,&nb_object,sizeof(int));
  for(int i = 0; i < nb_object; i++){
    frame = map_get_frames(i);
    strcpy(name,map_get_name(i));
    tmp = strlen(name);
    write(output,&tmp,sizeof(int));
    for(int j = 0; j < tmp; j++){
      write(output,&name[j],sizeof(int));
    }
    type1 = map_get_solidity(i);
    type2 = map_is_destructible(i);
    type3 = map_is_collectible(i);
    type4 = map_is_generator(i);
    write(output,&frame,sizeof(int));
    write(output,&type1,sizeof(int));
    write(output,&type2,sizeof(int));
    write(output,&type3,sizeof(int));
    write(output,&type4,sizeof(int));
  }
  //----------------------lecture de map-------------------------
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      valeur = map_get(x,y);
      err = write(output,&valeur,sizeof(int));
      if(err == -1){
	fprintf (stderr,"Problème de sauvegarde: %s\n",filename);
	exit(1);
      }
    }
  }
  close(output);
}

void map_load (char *filename)
{
  int err,fd;
  char n,type1,type2,type3,type4;
  char *adress = malloc(sizeof(char));
  //--------------ouverture du fichier de sauvgarde-----------------
  fd = open(filename, O_RDONLY ,0666);
  if(fd == -1){
    fprintf (stderr,"Désolé: %s n'existe pas\n",filename);
    exit(1);
  }
  err = lseek(fd,0,SEEK_SET);
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  //-----------------chargement de la taille-----------------------
  err = read(fd,&n, sizeof(int));
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  unsigned width = n;
  err = read(fd,&n, sizeof(int));
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  unsigned height= n;
  map_allocate (width, height);
  err = read(fd,&n, sizeof(int));
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  int nb_object = n;
  //------------------chargement des objets----------------------
  map_object_begin(nb_object);
  char *tmp1 = malloc(sizeof(char)*20);
  for(int i = 0; i < nb_object ; i++){
    err = read(fd,&adress[0], sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    tmp1 = realloc(tmp1,((int)adress[0])*sizeof(char));
    for(int j = 0; j < (int)adress[0];j++){
      err = read(fd,&tmp1[j], sizeof(int));
      if(err == -1){
	fprintf (stderr,"Problème de format: %s\n",filename);
	exit(1);
      }
    }
    err = read(fd,&n, sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    err = read(fd,&type1, sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    err = read(fd,&type2, sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    err = read(fd,&type3, sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    err = read(fd,&type4, sizeof(int));
    if(err == -1){
      fprintf (stderr,"Problème de format: %s\n",filename);
      exit(1);
    }
    if(type2){
      map_object_add ( tmp1, n, type1 | MAP_OBJECT_DESTRUCTIBLE);
    }
    else if(type3){
      map_object_add ( tmp1, n, type1 | MAP_OBJECT_COLLECTIBLE);
    }
    else if(type4){
      map_object_add ( tmp1, n, type1 | MAP_OBJECT_GENERATOR);
    }
    else{
      map_object_add ( tmp1, n, type1);
    }
    for(int k = 0; k < adress[0];k++){
      strcpy(tmp1+k,"");
    }
  }
  map_object_end ();
  free(tmp1);
  free(adress);
  //----------------------écriture de map-------------------------
  for (int y = 0; y < height ; y++){
    for (int x = 0; x < width ; x++){
      err = read(fd,&n, sizeof(int));
      if(err == -1){
	fprintf (stderr,"Problème de format: %s\n",filename);
	exit(1);
      }
      map_set (x, y, n);
    }
  }
  close(fd);
}

#endif
