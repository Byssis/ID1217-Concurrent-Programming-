/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0, max, min, mini, minj, maxi, maxj;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
    //  printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //	  printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }
  max = matrix[0][0];
  min = matrix[0][0];
  mini = 0;
  maxi = 0;
  minj = 0;
  maxj = 0;
  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total, min:min, max:max) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      int v = matrix[i][j];
      total += v;
      if(max < v){
        max = v;
        maxi = i;
        maxj = j;
      }
      if(min > v){
        min = v;
        mini = i;
        minj = j;
      }
    }

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("max %d, i = %d, j = %d\n", max, maxi, maxj);
  printf("min %d, i = %d, j = %d\n", min, mini, minj);
  printf("it took %g seconds\n", end_time - start_time);

}
