#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define MAXWORKERS 1
#define WORDLENGTH 20
pthread_mutex_t word_index_lock;

typedef struct node {
char* array;
int size;
} thread_args;

int word_index = 0;

int getIndex(){
  int i;
  pthread_mutex_lock(&word_index_lock);
  i = word_index++;
  pthread_mutex_unlock(&word_index_lock);
  return i;
}

int binarySearch(char * arr, int l, int r, char * x){
  printf("binarySearch: %d, %d, %s \n", l, r, x);
  while (l <= r){
    int m = l + (r-l)/2;
    int result = (int)strcmp(x, (arr + m));
    printf("strcmp: %d \n", result);
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
  thread_args* arg = (thread_args*)args;
  int size = (int)arg->size;
  char* a = (char *)arg->array;
  printf("Start working!!!\n");
  while (true) {
    // 1. Get word from bag
    int i = getIndex();
    printf("Start with index: %d \n", i);
    if(i >= size) break;
    // 2. flip word
    char * word = (char *) a + 1;
    printf("Word %s \n", word);
    char * flip = word;//strrev(word);
    // 3. search for word in word array
    int result = binarySearch(a, 0, size, flip );
    printf("binarySearch: %d \n", result);
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
  char line[WORDLENGTH];
  FILE* file = fopen(fileName, "r");
  int size = 0;

  while (fgets(line, sizeof(line), file)) {
    size++;
  }
  printf("Size %d\n", size);
  fclose(file);
  int i = 0;

  char * words [size];
  file = fopen(fileName, "r");
  printf("Read in words\n");
  while (fgets(line, sizeof(line), file)) {
    //printf("Index i: %d, Word: %s\n", i, line);
    words[i++] = line;
    printf("Index i: %d, Word: %s\n", i, words[i-1]);
  }
  fclose(file);
  printf("Words read\n");
  int numWorkers =  MAXWORKERS;
  pthread_t workerid[numWorkers];

  thread_args args;

  args.size = size;
  args.array = (char *) words;
  int l;
  printf("Start threads %d\n", args.size);
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++){
    printf("Start worker %d\n", l);
    pthread_create(&workerid[l], NULL, Worker, &args);
  }

  for (l = 0; l < numWorkers; l++)
    pthread_join(workerid[l],NULL);

  end_time = read_timer();

  printf("The execution time is %g sec\n", end_time - start_time);
}
