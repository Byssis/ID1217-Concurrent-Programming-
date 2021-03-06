#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bench.h"
#include "checksorted.h"

#define MAXSIZE 100000
#define MAXWORKERS 10

typedef struct node {
int* array;
int lo;
int hi;
} theard_args;

pthread_mutex_t num_workers;
int current_workers = 1;
int max_workers;


// Function delcaretion of co_quicksort
void * co_quicksort(void * arg);

void createThread(pthread_t* tid, int array[], int lo, int hi){
  theard_args n1;                           // args to new thread
  n1.array = array;                         // pointer to array
  n1.lo = lo;                               // lower bound
  n1.hi = hi;                               // Higher bound
  pthread_create(tid, NULL, co_quicksort, &n1);  // new thread
}

/*
  quicksort
  This function will sort an array of number with a concurrent implementation
  of quicksort. Quicksort splits the array into two sub arrays. This implementation
  will create a new thread for one of the sub arrays and continue with the other
  sub array in the current thread. If size of the sub array is less than MIN_SIZE
  then excute rest of the recursion sequential.

  int array[]   array to be sorted
  int lo        lower bound of sub array of arrry to be soretd
  int hi        higher bound of sub array of array to be sorted
*/
void quicksort(int array[], int lo, int hi){
  pthread_t tidl, tidr;
  int l = 0;
  int r = 0;
  if(lo >= hi) return;                        // Base case for recursion
  int p = partition(array, lo, hi);           // Get pivot element
  pthread_mutex_lock(&num_workers);
  if(current_workers < max_workers){          // Decide if to create a new thread
    current_workers++;
    pthread_mutex_unlock(&num_workers);
    createThread(&tidl, array, lo, p-1);
    l = 1;
  }
  else {
    pthread_mutex_unlock(&num_workers);
    quicksort(array, lo, p-1);                // normal quicksort
  }

  pthread_mutex_lock(&num_workers);
  if(current_workers < max_workers){          // Decide if to create a new thread
    current_workers++;
    pthread_mutex_unlock(&num_workers);
    createThread(&tidr, array, p+1, hi);
    r = 1;
  }
  else {
    pthread_mutex_unlock(&num_workers);
    quicksort(array, p+1, hi);
  }
  if(l)pthread_join(tidl, NULL);
  if(r)pthread_join(tidr, NULL);

}


/*
  co_quicksort
*/
void * co_quicksort(void * arg){
  theard_args * node = (theard_args *) arg;
  quicksort(node->array, node->lo, node->hi);
  pthread_mutex_lock(&num_workers);
  current_workers--;
  pthread_mutex_unlock(&num_workers);
}

int partition(int array[], int lo, int hi){
  int p = array[hi];
  int i = lo - 1;
  int j;
  for (j = lo; j < hi; j++){
    if( array [j] < p)
      swap(array, ++i, j);
  }
  if(array[hi] < array[i+1])
    swap(array, i+1, hi);
  return i + 1;
}

int swap(int array[], int p1, int p2 ){
  int temp = array[p2];
  array[p2] = array[p1];
  array[p1] = temp;
}

int main(int argc, char *argv[]){
  int size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  int max = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  int i;
  int j;

  for (j = 1; j < max; j = j * 2) {
    max_workers = j;
    int array[size];
    for (i = 0; i < size; i++)
      array[i]= rand() % 99;
    double start_time = read_timer();
    quicksort(array, 0, size-1);
    double end_time = read_timer();
    if(ifsorted(array, size) == 0){
      printf("Error\n");
    }
    printf("Threads: %d. The execution time is %g sec\n", j, end_time - start_time);
  }
}
