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
  fprintf(stderr,"%s <file> <option>\n",*arg);
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
  

int main(int argc, char *argv[])
{
  char *optTab[NB_OPTIONS];

  if(argc<4)
    usage(argv[0]);
  
  char *option=argv[2];
  optionsAlloc(optTab);

  for(int i=0; i<NB_OPTIONS;i++)
    printf("%d : %s\n",i,optTab[i]);
  printf("\nOption choisie :\t");
  if(*option==*optTab[0])
    printf("%s",*optTab[0]);
  if(*option==*optTab[1])
      printf("%s",*optTab[1]);
  if(*option==*optTab[2])
    printf("%s",*optTab[2]);
  if(*option==*optTab[3])
      printf("%s",*optTab[3]);
  if(*option==*optTab[4])
      printf("%s",*optTab[4]);
  if(*option==*optTab[5])
      printf("%s",*optTab[5]);
  if(*option==*optTab[6])
      printf("%s",*optTab[6]);
  if(*option==*optTab[7])
      printf("%s",*optTab[7]);
  else
      printf("Option inconnue!");
    }
  
  optionsFree(optTab);
  
  return EXIT_SUCCESS;
}

#endif
