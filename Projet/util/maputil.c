#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

#define NB_OPTIONS 8

//Alloue un tableau contenant toutes les options possibles de maputil
void optionsAlloc(char *t[])
{
  for(int i=0; i<NB_OPTIONS;i++)
    t[i]=malloc(14*sizeof(char));
  strcpy(t[0],"--getwidth");
  strcpy(t[1],"--getheight");
  strcpy(t[2],"--getobjects");
  strcpy(t[3],"--getinfo");
  strcpy(t[4],"--setwidth");
  strcpy(t[5],"--setheight");
  strcpy(t[6],"--setobjects");
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
  for(int i=0; i<NB_OPTIONS;i++)
    printf("%d : %s\n",i,optTab[i]);
  
  optionsAlloc(optTab);

  for(int i=0; i<NB_OPTIONS;i++)
    printf("%d : %s\n",i,optTab[i]);
  
  optionsFree(optTab);

  for(int i=0; i<NB_OPTIONS;i++)
    printf("%d : %s\n",i,optTab[i]);
  return EXIT_SUCCESS;
}

#endif
