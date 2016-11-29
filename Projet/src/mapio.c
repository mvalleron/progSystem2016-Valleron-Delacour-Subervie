#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN
https://github.com/mvalleron/progSystem2016-Valleron-Delacour-Subervie.git
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
  int valeur, err;
  int height = map_height(); 
  int width = map_width(); 
  int output = open("maps/saved.map", O_TRUNC | O_WRONLY | O_CREAT,0666);
  if(output == -1){
    fprintf (stderr,"Problème dans maps/saved.map: %s\n",filename);
    exit(1);
  }
  lseek(output, 1, SEEK_SET);
  write(output,&width,sizeof(int));
  lseek(output, 3, SEEK_SET);
  write(output,&height,sizeof(int));
  for(int y = 0; y < height; y++){
    for(int x = 0; x < width; x++){
      valeur = map_get(x,y);
      err = lseek(output, (x+(width*y))*2+5, SEEK_SET);
      if(err == -1){
	fprintf (stderr,"Problème de sauvegarde: %s\n",filename);
        exit(1);
      }
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
  char n;
  fd = open(filename, O_RDONLY ,0666);
  if(fd == -1){
    fprintf (stderr,"Désolé: %s n'existe pas\n",filename);
    exit(1);
  }
  err = lseek(fd,1,SEEK_SET);
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  err = read(fd,&n, sizeof(int));
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  unsigned width = n;
  err = lseek(fd,3,SEEK_SET);
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  err = read(fd,&n, sizeof(int));
  if(err == -1){
    fprintf (stderr,"Problème de format: %s\n",filename);
    exit(1);
  }
  unsigned height= n;
  map_new (width,height);
  for (int y = 0; y < height-1 ; y++){
    for (int x = 1; x < width-1 ; x++){
      err = lseek(fd,(y*width+x)*2+5,SEEK_SET);
      if(err == -1){
	fprintf (stderr,"Problème de format: %s\n",filename);
	exit(1);
      }
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
