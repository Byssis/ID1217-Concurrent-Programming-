/*
finding palindromic words in a dictionary using pthreads

   features: uses a number of worker(threads),
      each worker get task fram a bag of task(getIndex).

   usage under Linux:
     gcc -o palindromic palindromic.c -lpthread
     palindromic fileName numWorkers > result.txt
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bench.h"

#define MAXWORKERS 33
#define WORDLENGTH 40
#define TASKLENGTH 10
#define MAXSIZE 30000

pthread_mutex_t word_index_lock;
pthread_mutex_t sum_lock;
pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;

int word_index = 0;
int sum = 0;
int size, numArrived, numWorkers;
char dictionary[MAXSIZE][WORDLENGTH];

int getIndex(){
  int i;                                      // Temp variable to store index
  pthread_mutex_lock(&word_index_lock);       // Start Critical section
  i = word_index;                             // Store to temp variable
  word_index += TASKLENGTH;                   //
  pthread_mutex_unlock(&word_index_lock);     // End Critical section
  return i;
}

/*
  a reusable counter barrier
  Taken from matrixSum.c
 */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
  pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}


/*
    reverse string
    char * word, string to reverse
*/
void reverse(char * word, char * r){
  int i, j;
  i = 0;
  j = strlen(word) - 1;
  while (j >= 0) {
    r[i++] = word[j--];
  }
  r[i]  = '\0';
}

/*
  binarySearch for searching in the dictionary
*/
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
  long myid = (long) args;                    // Id of worker
  int partial_sum = 0;                        // Counter for numbers of words found
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

      // 3. search for fliped word in dictionary
      int result = binarySearch(0, size, flip);

      // 4. print if in dictionary
      if(result != -1){
        partial_sum++;
        //printf("%s %s\n", word, flip);
      }
    }
  }
  pthread_mutex_lock(&sum_lock);                // Enter Critical section
  sum += partial_sum;                           // Updating sum
  pthread_mutex_unlock(&sum_lock);              // Exits Critical section

  Barrier();                                    // Wait for other threads to finnish
  //printf("Worker %d found: %d\n", myid, partial_sum);
}

int main(int argc, char *argv[]){
  double start_time, end_time;
  int k = 0, i;
  long l;

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  if(argc < 3){
    printf("Error! Argument missing: file to examine\n");
    exit(0);
  }
  char const* const fileName = argv[1];
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;

  // Read dictionary
  FILE* file = fopen(fileName, "r");
  while(fscanf(file,"%s",dictionary[k]) == 1){
    for(i= 0; dictionary[k][i]; i++){
        dictionary[k][i] = tolower(dictionary[k][i]);
    }
    k++;
  }
  fclose(file);
  size = k - 1;

  for (i = 1; i <= MAXWORKERS; i = i*2) {
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  //i = numWorkers;
  sum = 0;
  word_index = 0;
  //printf("Size: %d\n", size);
  pthread_t workerid[i];

  start_time = read_timer();                  // Start time for benchmark

  // Spawn workers
  for (l = 0; l < i; l++)
    pthread_create(&workerid[l], NULL, Worker, (void *)l);

  // Wait for workers
  for (l = 0; l < i; l++)
    pthread_join(workerid[l], NULL);

  end_time = read_timer();                    // End time for benchmark

  // Print execution time
  printf("Num threads: %d. The execution time is %g sec. Num words: %d\n"
          , i, end_time - start_time, sum);
  }
}
