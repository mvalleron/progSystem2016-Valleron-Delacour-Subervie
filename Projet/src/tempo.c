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
int itterateur = 0;
void traitant(int s){
  printf ("sdl_push_event(%p) appelée au temps %ld\n", t[0].parametre, get_time ());
  sdl_push_event(t[0].parametre);
  for(int i = 0; i < itterateur+1; i++){
    t[i].temps = t[i+1].temps;
    t[i].parametre = t[i+1].parametre;
  }
  itterateur--;
}

void *f(void *i){
  sigset_t mask,empty_mask;
  sigemptyset(&mask);
  sigemptyset(&empty_mask);
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
  int i = 0;
  unsigned long time = (unsigned long)(delay*1000)+get_time();
  while(t[i].temps<time && i!=itterateur){
    i++;
  }
  int j = 0;
  while(i!=itterateur-j){
    int tmp_temps = t[itterateur-j-1].temps;
    void* tmp_param = t[itterateur-j-1].parametre;
    t[itterateur-j-1].temps = t[itterateur-j].temps;
    t[itterateur-j-1].parametre = t[itterateur-j].parametre;
    t[itterateur-j].temps = tmp_temps;
    t[itterateur-j].parametre = tmp_param;
    j++;
  }
  t[i].temps = time;
  t[i].parametre = param;
  itterateur++;
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
