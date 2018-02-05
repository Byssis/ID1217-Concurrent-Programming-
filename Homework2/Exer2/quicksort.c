/* quicksort of list OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o quicksort quicksort.c
     ./quicksort size numWorkers

*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXSIZE 100000
#define MAXWORKERS 16
double start_time, end_time;

void quicksort(int array[], int lo, int hi){
  if(lo >= hi) return;
  int p = partition(array, lo, hi);

  #pragma omp parallel sections |
  {
    #pragma omp section
      quicksort(array, lo, p-1);
    #pragma omp section
      quicksort(array, p+1, hi);
  }
}

int partition(int array[], int lo, int hi){
  int p = array[hi];
  int i = lo - 1;
  int j;
  for (j = lo; j < hi; j++){
    if( array [j] < p )
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
  int numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  omp_set_num_threads(numWorkers);
  int array[size];
  int i;
  for (i = 0; i < size; i++)
    array[i]= rand() % 99;
  //printf("%d\n", size);
  double start_time = omp_get_wtime();
  quicksort(array, 0, size-1);
  double end_time = omp_get_wtime();

  printf("The execution time is %g sec\n", end_time - start_time);
}
