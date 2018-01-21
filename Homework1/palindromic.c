/*
finding palindromic words in a dictionary using pthreads

   features: uses a number of worker(threads),
      each worker get task fram a bag of task(getIndex).

   usage under Linux:
     gcc -o palindromic palindromic.c -lpthread
     palindromic fileName
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bench.h"

#define MAXWORKERS 10
#define WORDLENGTH 40
#define TASKLENGTH 10
#define MAXSIZE 25143
pthread_mutex_t word_index_lock;

int word_index = 0;

char dictionary[MAXSIZE][WORDLENGTH];

int getIndex(){
  int i;
  pthread_mutex_lock(&word_index_lock);
  i = word_index;
  word_index += TASKLENGTH;
  pthread_mutex_unlock(&word_index_lock);
  return i;
}

void reverse(char * word, char * r){
  int i, j;
  i = 0;
  j = strlen(word) - 1;
  while (j >= 0) {
    r[i++] = word[j--];
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
    int j;
    for (j = 0; j < TASKLENGTH && (i + j) < size; j++) {
      // 2. flip word
      char * word = dictionary[i + j];
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
}

int main(int argc, char *argv[]){
  double start_time, end_time;
  int k, l, size;
  if(argc < 2){
    printf("Error! Argument missing: file to examine\n");
    exit(0);
  }

  char const* const fileName = argv[1];

  FILE* file = fopen(fileName, "r");
  for (k = 0; k < MAXSIZE; k++) {
    fscanf(file, "%s", dictionary[k]);
  }
  fclose(file);
  size = k + 1;

  int numWorkers =  MAXWORKERS;
  pthread_t workerid[numWorkers];

  start_time = read_timer();                  // Start time for benchmark

  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], NULL, Worker, &size);

  for (l = 0; l < numWorkers; l++)
    pthread_join(workerid[l],NULL);

  end_time = read_timer();

  printf("The execution time is %g sec\n", end_time - start_time);
}
