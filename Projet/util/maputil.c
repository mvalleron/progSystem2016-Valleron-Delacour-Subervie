#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

#define NB_OPTIONS 8

static void usage(char *arg)
{
  fprintf(stderr,"%s <file> <option>\n",arg);
  exit(EXIT_FAILURE);
}
  

//Alloue un tableau contenant toutes les options possibles de maputil
void optionsAlloc(char *t[])
{
  t[0]=malloc(10*sizeof(char));
  strcpy(t[0],"--getwidth");
  t[1]=malloc(11*sizeof(char));
  strcpy(t[1],"--getheight");
  t[2]=malloc(12*sizeof(char));
  strcpy(t[2],"--getobjects");
  t[3]=malloc(9*sizeof(char));
  strcpy(t[3],"--getinfo");
  t[4]=malloc(10*sizeof(char));
  strcpy(t[4],"--setwidth");
  t[5]=malloc(11*sizeof(char));
  strcpy(t[5],"--setheight");
  t[6]=malloc(12*sizeof(char));
  strcpy(t[6],"--setobjects");
  t[7]=malloc(14*sizeof(char));
  strcpy(t[7],"--pruneobjects");
}

//Libère le tableau contenant toutes les options de maputil
void optionsFree(char *t[])
{
  for(int i=0; i<NB_OPTIONS;i++)
    free(t[i]);
}

//Renvoie la largeur d'une carte
int getWidth(int Fd)
{
  int i;
  int e=lseek(Fd,0,SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  e=read(Fd,&i,sizeof(int));
  if(e==-1)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }
  return i;
}

//Renvoie la hauteur d'une carte
int getHeight(int Fd)
{
  int i;
  int e=lseek(Fd,sizeof(int),SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  e=read(Fd,&i,sizeof(int));
  if(e==-1)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }
  return i;
}

//Renvoie le nombre d'objets d'une carte
int getObjects(int Fd)
{
  int i;
  int e=lseek(Fd,2*sizeof(int),SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  e=read(Fd,&i,sizeof(int));
  if(e==-1)
    {
      perror("read");
      exit(EXIT_FAILURE);
    }
  return i;
}

//Change l'ancienne largeur par la nouvelle w
void setWidth(int Fd,int w)
{
  int oldW=getWidth(Fd);

  if(oldW==w)
    break;
   
  int h=getHeight(Fd);
  int j=0;
  int t[h*w];
  
  int e=lseek(Fd,0,SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  e=write(Fd,&w,sizeof(int));
  if(e==-1)
    {
      perror("write");
      exit(EXIT_FAILURE);
    }
  e=lseek(Fd,3*sizeof(int),SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  //Recopie des éléments communs aux nouvelles et anciennes tailles
  for(int y=0;y<h;y++)
    {
      for(int x=0;x<w;x++)
	{
	  e=read(Fd,t+j,sizeof(int));
	  if(e==-1)
	    {
	      perror("read");
	      exit(EXIT_FAILURE);
	    }
	  j++;
	}
      //Si la taille est rétrécie
      if(oldW>W)
	lseek(Fd,(oldW-w)*sizeof(int),SEEK_CUR);
      //Si la taille est augmentée
      if(oldW<w)
	{
	  for(k=j;k<j+(w-oldW);k++)
	    t[k]=MAP_OBJECT_NONE;
	  j=k;
	}
    }
  //Place le curseur au début de la liste des objets
  e=lseek(Fd,3*sizeof(int),SEEK_SET);
  if(e==-1)
    {
      perror("lseek");
      exit(EXIT_FAILURE);
    }
  //Ecrit les éléments du tableau dans le fichier Fd
  for(int y=0;y<h;y++)
    {
      for(int x=0;x<w;x++)
	{
	  e=write(Fd,t+(y+x),sizeof(int));
	  if(e==-1)
	    {
	      perror("write");
	      exit(EXIT_FAILURE);
	    }
	}
    }
  //Tronque le fichier s'il est plus petit
  if(oldW>w)
    truncate(Fd,(h*(oldW-w))*sizeof(int));
}


//Teste les différents arguments et appelle la fonction correspondante si c'est le cas
int traitementOption(char *optTab[],int Fd, char *option){
  int n=0;
  printf("\nOption choisie :\t");
  if(!strcmp(option,optTab[0]))
    {
      printf("%s\t",optTab[0]);
      printf("%d\n",getWidth(Fd));
      n=1;
    }
  else if(!strcmp(option,optTab[1]))
    {
      printf("%s\t",optTab[1]);
      printf("%d\n",getHeight(Fd));
      n=1;
    }
  else if(!strcmp(option,optTab[2]))
    {
      printf("%s\t",optTab[2]);
      printf("%d\n",getObjects(Fd));
      n=1;
    }
  else if(!strcmp(option,optTab[3]))
    {
      printf("%s\t",optTab[3]);
      printf("Largeur : %d\tHauteur : %d\tNombre d'objets : %d\n",getWidth(Fd),getHeight(Fd),getObjects(Fd));
      n=1;
    }
  else if(!strcmp(option,optTab[4]))
    {
      printf("%s",optTab[4]);
      n=2;
    }
  else if(!strcmp(option,optTab[5]))
    {
      printf("%s",optTab[5]);
      n=2;
    }
  else if(!strcmp(option,optTab[6]))
    {
      printf("%s",optTab[6]);
      n=2;
    }
  else if(!strcmp(option,optTab[7]))
    {
      printf("%s",optTab[7]);
      n=1;
    }
  else
    {
      printf("Option inconnue!");
      n=1;
    }
  printf("\n");
  return n;
}

int main(int argc, char *argv[])
{
  char *optTab[NB_OPTIONS];
  int k=2;
  int n,Fd;
  char *option;
  
  if(argc<3)
    usage(argv[0]);

  Fd=open(argv[1],O_RDWR);
  if(Fd==-1)
    {
      perror("open");
      exit(EXIT_FAILURE);
    }
  optionsAlloc(optTab);

  for(int i=0; i<NB_OPTIONS;i++)
    printf("%d : %s\n",i,optTab[i]);
  
  while (k<argc)
    {
      option=argv[k];
      n =traitementOption(optTab,Fd,option); //n nb arg utilisé
      k+= n;
    }
  
  optionsFree(optTab);
  
  return EXIT_SUCCESS;
}

#endif
