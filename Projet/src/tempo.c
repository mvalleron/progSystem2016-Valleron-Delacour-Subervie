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
void traitant(int s){
  printf("signal %d\n", s);
}

void *f(void *i){
  sigset_t mask,empty_mask;
  sigemptyset(&mask);
  sigemptyset(&empty_mask);
  struct sigaction s;
  s.sa_handler = traitant;
  sigemptyset(&s.sa_mask);
  s.sa_flags=0;
  sigaction(SIGINT,&s,NULL);
  printf("ok\n");
  while(1){
    sigsuspend(&empty_mask);
  }
}
// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
  printf("ok\n");
  pthread_t pid;
  pthread_create(&pid,NULL,f,(void *)pid);
  return 0; // Implementation not ready
}

void timer_set (Uint32 delay, void *param)
{
  // TODO
}

#endif
