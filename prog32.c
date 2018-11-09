#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <pthread.h>
#include <semaphore.h>

#define ROG_CNT 15

sem_t chopstick[ROG_CNT];
long eat[ROG_CNT] = {0};

void *report(void *arg) {
     for(;;) {
          int i;
          for(i = 0; i < ROG_CNT; i++) {
               printf("%8lX ", eat[i]);
          }
          printf("\n");
          usleep(10000);
     }

     return NULL;
}

void *roger(void *arg) {
     int r = (int)(intptr_t)arg;

     for(;;) {
        if(sem_trywait(&(chopstick[r])) == 0)
        {
          if(sem_trywait(&(chopstick[(r + 1) % ROG_CNT])) == 0)
          {
            eat[r]++;

            sem_post(&(chopstick[(r + 1) % ROG_CNT]));
          }
          sem_post(&(chopstick[r]));
        }
     }

     return NULL;
}

int main(int argc, char *argv[]) {

     pthread_t rog[ROG_CNT];
     pthread_t t;
     int i;

     pthread_create(&t, NULL, report, NULL);

     for(i = 0; i < ROG_CNT; i++) {
          sem_init(&(chopstick[i]), 0, 1);
     }

     for(i = 0; i < ROG_CNT; i++) {
          pthread_create(&(rog[i]), NULL, roger, (void*)(intptr_t)i);
     }

     for(i = 0; i < ROG_CNT; i++) {
          pthread_join(rog[i], NULL);
     }

     for(i = 0; i < ROG_CNT; i++) {
          sem_destroy(&(chopstick[i]));
     }

     return EXIT_SUCCESS;
}
