#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bench.h"

#define MAXWORKERS 10
#define WORDLENGTH 40
#define MAXSIZE 25143
pthread_mutex_t word_index_lock;

int word_index = 0;

char dictionary[MAXSIZE][WORDLENGTH];

int getIndex(){
  int i;
  pthread_mutex_lock(&word_index_lock);
  i = word_index++;
  pthread_mutex_unlock(&word_index_lock);
  return i;
}

void reverse(char * word, char * r){
  int i, j;
  i = 0;
  j = strlen(word) - 1;
  while (j >= 0) {
    r[i++] = word[j++];
  }
  r[i]  = '\0';
}

int binarySearch(int l, int r, char * x){
  while (l <= r){
    int m = l + (r-l)/2;
    int result = (int)strcmp(x, dictionary[m]);
    if (result == 0)
      return m;
    if (result > 0)
      l = m + 1;
    else
      r = m - 1;
  }
  return -1;
}

void * Worker(void * args){
  int size = *((int *)args);

  while (true) {
    // 1. Get word from bag
    int i = getIndex();
    if(i >= size) break;
    // 2. flip word
    char * word = dictionary[i];
    char flip[WORDLENGTH];
    reverse(word, flip);

    // 3. search for word in word array
    int result = binarySearch(0, size, flip);
    //printf("binarySearch: %d \n", result);
    // 4. print if
    if(result != -1)
      printf("%s %s\n", word, flip);
  }
}

int main(int argc, char *argv[]){
  double start_time, end_time;
  int k, l, size;
  printf("Palindromic words: \n");
  if(argc < 2){
    printf("Error! Argument missing: file to examine\n");
    exit(0);
  }

  char const* const fileName = argv[1];

  FILE* file = fopen(fileName, "r");
  for (k = 0; k < MAXSIZE; k++) {
    fscanf(file, "%s", dictionary[k]);
  }
    printf("WTF!!!\n");
  size = k + 1;
  fclose(file);

  int numWorkers =  MAXWORKERS;
  pthread_t workerid[numWorkers];
  printf("WTF!!!\n");
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++){
    printf("Worker: %d\n", l);
    pthread_create(&workerid[l], NULL, Worker, &size);
  }

  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l],NULL);
  }
  end_time = read_timer();

  printf("The execution time is %g sec\n", end_time - start_time);
}
