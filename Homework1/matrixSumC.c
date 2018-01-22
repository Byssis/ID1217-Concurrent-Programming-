/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */


pthread_mutex_t sum;  /* mutex lock for the barrier */
pthread_mutex_t max;      /* mutex lock for maximum value */
pthread_mutex_t min;      /* mutex lock for minimal value */
pthread_mutex_t bag;
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */

int maxValue, maxI, maxJ;
int minValue, minI, minJ;
int sumValue;

int row = 0;

void UpdateMax(int value, int i, int j){
  pthread_mutex_lock(&max);
  if(value > maxValue){
    maxValue = value;
    maxI = i;
    maxJ = j;
  }
  pthread_mutex_unlock(&max);
}

void UpdateMin(int value, int i, int j){
  pthread_mutex_lock(&min);
  if(value < minValue){
    minValue = value;
    minI = i;
    minJ = j;
  }
  pthread_mutex_unlock(&min);
}

void UpdateSum(int value){
  pthread_mutex_lock(&sum);
  sumValue += value;
  pthread_mutex_unlock(&sum);
}

int getTaskFromBag(){
  int b;
  pthread_mutex_lock(&bag);
  b = row++;
  pthread_mutex_unlock(&bag);
  return b;
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

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], NULL, Worker, (void *) l);

  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l],NULL);
  }
  end_time = read_timer();

  printf("Max=%d, i=%d, j=%d\n", maxValue, maxI, maxJ);
  printf("Min=%d, i=%d, j=%d\n", minValue, minI, minJ);
  printf("The total is %d\n", sumValue);
  printf("The execution time is %g sec\n", end_time - start_time);
}

void *Worker(void *arg) {
  int i, j, wmin, wminI, wminJ, wmax, wmaxI, wmaxJ, total;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  while (true) {
    i = getTaskFromBag();
    if(i >= size) break;
    total = 0;
    wmin = matrix[0][0];
    wmax = matrix[0][0];

    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j] > wmax){
        wmax = matrix[i][j];
        wmaxI = i;
        wmaxJ = j;
      }
      if(matrix[i][j] <= wmin){
        wmin = matrix[i][j];
        wminI = i;
        wminJ = j;
      }
      UpdateMax(wmax, wmaxI, wmaxJ);
      UpdateMin(wmin, wminI, wminJ);
      UpdateSum(total);
    }
  }
}
