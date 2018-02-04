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

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_mutex_t sum;  /* mutex lock for the barrier */
pthread_mutex_t max;      /* mutex lock for maximum value */
pthread_mutex_t min;      /* mutex lock for minimal value */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */

int maxValue, maxI, maxJ;
int minValue, minI, minJ;
int sumValue;

int first = 1;

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
  if(value <= minValue || first){
    minValue = value;
    minI = i;
    minJ = j;
    first = 0;
  }
  pthread_mutex_unlock(&min);
}

void UpdateSum(int value){
  pthread_mutex_lock(&sum);
  sumValue += value;
  pthread_mutex_unlock(&sum);
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

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

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
    pthread_create(&workerid[l], &attr, Worker, (void *) l);

  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l],NULL);
  }

  end_time = read_timer();

  printf("Max=%d, i=%d, j=%d\n", maxValue, maxI, maxJ);
  printf("Min=%d, i=%d, j=%d\n", minValue, minI, minJ);
  printf("The total is %d\n", sumValue);
  printf("The execution time is %g sec\n", end_time - start_time);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int total, i, j, first, last, wmin, wminI, wminJ, wmax, wmaxI, wmaxJ;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  /* sum values in my strip */
  total = 0;
  wmin = matrix[0][0];
  wmax = matrix[0][0];
  for (i = first; i <= last; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j] > wmax){
        wmax = matrix[i][j];
        wmaxI = i;
        wmaxJ = j;
      }
      if(matrix[i][j] < wmin){
        wmin = matrix[i][j];
        wminI = i;
        wminJ = j;
      }
  }
  UpdateMax(wmax, wmaxI, wmaxJ);
  UpdateMin(wmin, wminI, wminJ);
  UpdateSum(total);
}
