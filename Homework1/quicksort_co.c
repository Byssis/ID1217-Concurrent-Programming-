#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bench.h"

#define MAXSIZE 100000
#define MAXWORKERS 10

typedef struct node {
int* array;
int lo;
int hi;
} theard_args;

pthread_mutex_t num_workers;
int current_workers = 0;
int max_workers;


// Function delcaretion of co_quicksort
void * co_quicksort(void * arg);

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
  printf("lo: %d, hi: %d\n", lo, hi);

  pthread_t tidl, tidr;
  if(lo >= hi) return;                        // Base case for recursion
  int p = partition(array, lo, hi);           // Get pivot element
  pthread_mutex_lock(&num_workers);
  if(current_workers < max_workers){                       // Decide if to create a new thread
    current_workers++;
    printf("%d\n", current_workers);
    pthread_mutex_unlock(&num_workers);
    theard_args n1;                           // args to new thread
    n1.array = array;                         // pointer to array
    n1.lo = lo;                               // lower bound
    n1.hi = p-1;                              // Higher bound
    pthread_create(&tidl, NULL, co_quicksort, &n1);  // new thread
  }
  else {
    pthread_mutex_unlock(&num_workers);
    quicksort(array, lo, p-1);                // normal quicksort
  }

  pthread_mutex_lock(&num_workers);
  if(current_workers < max_workers){                       // Decide if to create a new thread
    current_workers++;
    printf("%d\n", current_workers);
    pthread_mutex_unlock(&num_workers);
    theard_args n1;                           // args to new thread
    n1.array = array;                         // pointer to array
    n1.lo = p+1;                               // lower bound
    n1.hi = hi;                              // Higher bound
    pthread_create(&tidr, NULL, co_quicksort, &n1);  // new thread
  }
  else {
    pthread_mutex_unlock(&num_workers);
    quicksort(array, p+1, hi);
  }
  pthread_join(tidl, NULL);
  pthread_join(tidr, NULL);
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
  int i   = lo - 1;
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
  max_workers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
  printf("Size: %d\n", size);
  printf("Workers: %d\n", max_workers);
  int i;
  int array[size];
  for (i = 0; i < size; i++)
    array[i]= rand() % 99;
  double start_time = read_timer();
  quicksort(array, 0, size-1);
  printf("Done!\n");
  double end_time = read_timer();
  /*for (i = 0; i < ARRAY_SIZE-1; i++) {
    printf("%d, ", array[i]);
  }*/
  printf("The execution time is %g sec\n", end_time - start_time);
}
