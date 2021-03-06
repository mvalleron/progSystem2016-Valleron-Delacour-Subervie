#define _XOPEN_SOURCE 700

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include "timer.h"

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;
  
  return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN
struct evenement{
  void* parametre;
  unsigned long temps;
};

struct evenement t[100];

int compteur = 0;

void trie(struct evenement t[], int compteur){
  void* tmpparametre=0; 
  unsigned long tmptemps=0;
  for(int i = 0; i < compteur; i++)                               
    {                                                                          
      for(int j = i+1; j < compteur; j++)                     
        {                                                 
          if(t[j].temps < t[i].temps)                                                        
            {                                                                  
              tmpparametre = t[i].parametre;
	      tmptemps = t[i].temps;
              t[i].parametre = t[j].parametre;
	      t[i].temps = t[j].temps;
              t[j].parametre = tmpparametre;
	      t[j].temps = tmptemps;
            }                                                                  
        }                                                                      
    } 
}

void traitant(int s){
  printf ("sdl_push_event(%p) appelée au temps %ld\n", t[0].parametre, get_time ());
  sdl_push_event(t[0].parametre);
  for(int i = 0; i < compteur+1; i++){
    t[i].temps = t[i+1].temps;
    t[i].parametre = t[i+1].parametre;
  }
  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  timer.it_value.tv_sec = (t[0].temps-get_time())/1000000; 
  timer.it_value.tv_usec = (t[0].temps-get_time())%1000000;  
  int err = setitimer(ITIMER_REAL,&timer,0);
  if(err){
    perror("setitimer");
    exit(1);
  }
  compteur--;
}

void *f(void *i){
  sigset_t mask,empty_mask;
  sigemptyset(&mask);
  sigemptyset(&empty_mask);
  sigaddset(&mask, SIGALRM);
  sigprocmask(SIG_BLOCK,&mask,NULL);
  
  struct sigaction s;
  s.sa_handler = traitant;
  sigemptyset(&s.sa_mask);
  s.sa_flags=0;
  sigaction(SIGALRM,&s,NULL);
  while(1){
    sigsuspend(&empty_mask);
  }
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  pthread_t pid = (pthread_t)NULL;
  pthread_create(&pid,NULL,f,(void *)pid);
  return 1; // Implementation ready
}

void timer_set (Uint32 delay, void *param)
{
  unsigned long time = (unsigned long)(delay*1000)+get_time();
  t[compteur].temps = time;
  t[compteur].parametre = param;
  compteur++;
  trie(t,compteur);
  struct itimerval timer;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  timer.it_value.tv_sec = (t[0].temps-get_time())/1000000; 
  timer.it_value.tv_usec = (t[0].temps-get_time())%1000000;  
  int err = setitimer(ITIMER_REAL,&timer,0);
  if(err){
    perror("setitimer");
    exit(1);
  }
}

#endif

//timerset :enfiler parametre, trié, timer init
//traitant :trié, timer init
