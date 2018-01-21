#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define MAXWORKERS 4
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

char * reverse(char * word){
  char r[WORDLENGTH];
  int i, j;
  i = 0;
  j = strlen(word) - 1;
  while (j >= 0) {
    r[i++] = *(word + j--);
  }
  //r[i++] = '\n';
  r[i] = '\0';
  return (char *)r;
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

  printf("Start working!!!\n");
  while (true) {
    // 1. Get word from bag
    int i = getIndex();
    //printf("Start with index: %d \n", i);
    if(i >= size) break;
    // 2. flip word
    char * word = dictionary[i];
    char * flip = reverse(word);
    //printf("Word %s %s \n", word, flip);
    // 3. search for word in word array
    int result = binarySearch(0, size, flip);
    //printf("binarySearch: %d \n", result);
    // 4. print if
    if(result != -1)
      printf("%s %s\n", word, flip);
  }
}

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]){
  printf("Start\n");
  double start_time, end_time;
  char const* const fileName = argv[1];

  FILE* file = fopen(fileName, "r");
  int size = 0;
  int k;
  for (k = 0; k < MAXSIZE; k++) {
    fscanf(file, "%s", dictionary[k]);    
  }
  size = k + 1;
  printf("Size %d\n", size);
  fclose(file);
  printf("%s\n", reverse(dictionary[0]) );
  printf("Words read\n");
  int numWorkers =  MAXWORKERS;
  pthread_t workerid[numWorkers];

  int l;
  printf("Start threads\n");
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++){
    printf("Start worker %d\n", l);
    pthread_create(&workerid[l], NULL, Worker, &size);
  }

  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l],NULL);
  }

  end_time = read_timer();

  printf("The execution time is %g sec\n", end_time - start_time);
}
