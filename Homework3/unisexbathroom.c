#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t bathroom;         // semaphore for bathroom
sem_t men;              // binary semaphore for men
sem_t women;            // binary semaphore for women
sem_t critical;

int delayedMen = 0;
int delayedWomen = 0;

int numberOfShittingMen = 0;
int numberOfShittingWoman = 0;

#define SHITTIME 2
#define WAITTIME 2

typedef struct threadArgs{
  int id;
  int poop;
}threadArgs;


void * Man(void * arg){

  struct threadArgs * a = (threadArgs *) arg;
  int id = a->id;
  int poop = a->poop;

  while (poop > 0) {
    // 1. Wait random time
    sleep(rand() % WAITTIME);
    // 2. Go to bathroom
    sem_wait(&bathroom);
    // Take a shit
    //printf("Man %d is now pooping\n", id);
    sleep(rand() % WAITTIME);
    //printf("Man %d finnised pooping\n", id);
    poop--;
    printf("Man %d has %d poops left\n", id, poop);
    // leave bathroom
    sem_post(&bathroom);
  }
  printf("Man %d is DONE!\n", id);
}

void * Woman(void * arg){
  struct threadArgs * a = (threadArgs *) arg;

  int id = a->id;
  int poop = a->poop;
  while (poop > 0) {
    // 1. Wait random time
    sleep(rand() % SHITTIME);
    // 2. Go to bathroom


    sem_wait(&bathroom);
    // Take a shit, random time
    //printf("Woman %d is now pooping\n", id);
    sleep(rand() % WAITTIME);
    //printf("Woman %d finnised pooping\n", id);
    poop--;
    printf("Woman %d has %d poops left\n", id, poop);
    // leave bathroom
    sem_post(&bathroom);


  }
  printf("Woman %d is DONE!\n", id);
}

int main(int argc, char* argv[]){
  int sizeOfBathroom = (argc > 1) ? atoi(argv[1]) : 6;
  int numOfMen = (argc > 2) ? atoi(argv[2]) : 6;
  int numOFWomen = (argc > 3) ? atoi(argv[3]) : 6;
  int l;
  pthread_t men[numOfMen];
  pthread_t women[numOFWomen];
  sem_init(&bathroom, 0, sizeOfBathroom);

  printf("Start pooping\n");
  printf("Create %d men\n", numOfMen);
  for (l = 0; l < numOfMen; l++){
    struct threadArgs args;
    args.id = l;
    args.poop = 10;
    pthread_create(&men[l], NULL, Man, (void *) &args);
  }

  printf("Create %d women\n", numOFWomen);
  for (l = 0; l < numOFWomen; l++){
    struct threadArgs args;
    args.id = l;
    args.poop = 10;
    pthread_create(&women[l], NULL, Woman, (void *) &args);
  }

  for (l = 0; l < numOfMen; l++)
    pthread_join(men[l],NULL);
  for (l = 0; l < numOFWomen; l++)
    pthread_join(women[l],NULL);

  printf("All men and women are done pooping!!!\n");


}
