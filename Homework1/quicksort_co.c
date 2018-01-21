#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 10000000
#define MIN_SIZE 10

int array [ARRAY_SIZE];

typedef struct node {
int* array;
int lo;
int hi;
} theard_args;

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
  if(lo >= hi) return;                        // Base case for recursion
  int p = partition(array, lo, hi);           // Get pivot element
  if(lo-hi > MIN_SIZE){                       // Decide if to create a new thread
    theard_args n1;                           // args to new thread
    n1.array = array;                         // pointer to array
    n1.lo = lo;                               // lower bound
    n1.hi = p-1;                              // Higher bound
    pthread_t tid;                            // to hold id to thread
    pthread_create(&tid, NULL, co_quicksort, &n1);  // new thread
    quicksort(array, p+1, hi);                // continue in this thread
    pthread_join(tid, NULL);                  // wait for thread to finnish
  }
  else {
    quicksort(array, lo, p-1);                // normal quicksort
    quicksort(array, p+1, hi);
  }
}

/*
  co_quicksort


*/
void * co_quicksort(void * arg){
  theard_args * node = (theard_args *) arg;
  quicksort(node->array, node->lo, node->hi);
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

int main(){
  int i;
  for (i = 0; i < ARRAY_SIZE-1; i++)
    array[i]= rand() % 99;
  quicksort(array, 0, ARRAY_SIZE-1);
  for (i = 0; i < ARRAY_SIZE-1; i++) {
    printf("%d, ", array[i]);
  }
}
