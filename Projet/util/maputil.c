#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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
  if(16<=w && w<=1024)
    {
      int oldW=getWidth(Fd);
      
      if(oldW==w)
	return;
      
      int h=getHeight(Fd);
      int j=0;
      int lenName;
      int k;
      int t[h*w];
      int nbObjects=getObjects(Fd);
      int nbCaracObj=5;
      
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
      //Place le curseur au début de la liste des objets
      e=lseek(Fd,3*sizeof(int),SEEK_SET);
      if(e==-1)
	{
	  perror("lseek");
	  exit(EXIT_FAILURE);
	}
      //Récupère la taille du nom de chaque fichier dans lenName, et décale de cette taille plus le nombre de caractéristiques des objets
      for(int i=0;i<nbObjects;i++)
	{
	  e=read(Fd,&lenName,sizeof(int));
	  if(e==-1)
	    {
	      perror("read");
	      exit(EXIT_FAILURE);
	    }
	  e=lseek(Fd,(lenName+nbCaracObj)*sizeof(int),SEEK_SET);
	  if(e==-1)
	    {
	      perror("lseek");
	      exit(EXIT_FAILURE);
	    }
	}
      //Recopie des éléments communs aux nouvelles et anciennes tailles
      for(int y=0;y<h;y++)
	{
	  for(int x=0;x<oldW;x++)
	    {
	      e=read(Fd,t+(y*oldW+x),sizeof(int));
	      if(e==-1)
		{
		  perror("read");
		  exit(EXIT_FAILURE);
		}
	      j++;
	    }
	  //Si la taille est rétrécie
	  if(oldW>w)
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
      //Récupère la taille du nom de chaque fichier dans lenName, et décale de cette taille plus le nombre de caractéristiques des objets
      for(int i=0;i<nbObjects;i++)
	{
	  e=read(Fd,&lenName,sizeof(int));
	  if(e==-1)
	    {
	      perror("read");
	      exit(EXIT_FAILURE);
	    }
	  e=lseek(Fd,(lenName+nbCaracObj)*sizeof(int),SEEK_SET);
	  if(e==-1)
	    {
	      perror("lseek");
	      exit(EXIT_FAILURE);
	    }
	}
      //Ecrit les éléments du tableau dans le fichier Fd
      for(int y=0;y<h;y++)
	{
	  for(int x=0;x<w;x++)
	    {
	      e=write(Fd,t+(y*w+x),sizeof(int));
	      if(e==-1)
		{
		  perror("write");
		  exit(EXIT_FAILURE);
		}
	    }
	}
      //Tronque le fichier s'il est plus petit
      if(oldW>w)
	{
	  int offset=lseek(Fd,0,SEEK_CUR);
	  ftruncate(Fd,offset);
	}
    }
  else
    printf("Nouvelle largeur non autorisee!\n");
}


//Teste la correspondance entre l'option demandée et les options existantes, et appelle une fonction correspondante si elle existe
int traitementOption(char *optTab[],int Fd, char *argv[],int k)
{
  char *option=argv[k];
  char *arg=argv[k+1];
  int n=0;
  printf("\nOption choisie :\t");
  //getwidth
  if(!strcmp(option,optTab[0]))
    {
      printf("%s\t",optTab[0]);
      printf("%d\n",getWidth(Fd));
      n=1;
    }
  //getheight
  else if(!strcmp(option,optTab[1]))
    {
      printf("%s\t",optTab[1]);
      printf("%d\n",getHeight(Fd));
      n=1;
    }
  //getobjects
  else if(!strcmp(option,optTab[2]))
    {
      printf("%s\t",optTab[2]);
      printf("%d\n",getObjects(Fd));
      n=1;
    }
  //getinfo
  else if(!strcmp(option,optTab[3]))
    {
      printf("%s\t",optTab[3]);
      printf("Largeur : %d\tHauteur : %d\tNombre d'objets : %d\n",getWidth(Fd),getHeight(Fd),getObjects(Fd));
      n=1;
    }
  //setwidth
  else if(!strcmp(option,optTab[4]))
    {
      printf("%s\t",optTab[4]);
      int w=atoi(arg);
      printf("%d\n",w);
      setWidth(Fd,w);
      n=2;
    }
  //setheight
  else if(!strcmp(option,optTab[5]))
    {
      printf("%s",optTab[5]);
      n=2;
    }
  //setobjects
  else if(!strcmp(option,optTab[6]))
    {
      printf("%s",optTab[6]);
      n=2;
    }
  //pruneobjects
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
  char *fichier = argv[1];
  char *fonction = argv[2];
  int nombre = atoi(argv[3]);
  printf("\n%s %s %d\n",fichier,fonction,nombre);
  char *optTab[NB_OPTIONS];
  int k=2;
  int n,Fd;
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

  //k est la position de l'option traitée dans argv
  while (k<argc)
    {
      n =traitementOption(optTab,Fd,argv,k); //n est le nombre d'arguments utilisés
      k+= n;
      if(k>=2)
	{
	  printf("maputil ne gere actuellement qu'une seule option a la fois!\n");
	  break;
	}
    }
  optionsFree(optTab);
  
  return EXIT_SUCCESS;
}

#endif
