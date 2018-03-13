#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define GRIDSIZE 10
#define NUMITERS 10
#define BOUNDRYCONSTANT 1.0

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



int main(int argc, char const *argv[]) {
  int gridSize, numIters, i, j, iter;

  gridSize = (argc > 1) ? atoi(argv[1]) : GRIDSIZE;
  numIters = (argc > 2) ? atoi(argv[2]) : NUMITERS;

  double ** grid = (double**)malloc(gridSize*sizeof(double));
  double ** new =  (double **)malloc(gridSize*sizeof(double));
  for (i = 0; i < gridSize; i++) {
    grid[i] = (double *)malloc(gridSize*sizeof(double));
    new[i] = (double *)malloc(gridSize*sizeof(double));
    for (j = 0; j < gridSize; j++) {
      if(i == 0 || j == 0 || i == gridSize - 1 || j == gridSize - 1){
        grid[i][j] = BOUNDRYCONSTANT;
        new[i][j] = BOUNDRYCONSTANT;
      }
      else{
        grid[i][j] = 0;
        new[i][j] = 0;
      }
    }
  }
  double start_time = read_timer();
  for(iter = 0; iter < numIters; iter++){
    for (i = 1; i < gridSize - 1; i++) {
      for (j = 1; j < gridSize - 1; j++) {
        new[i][j] = (grid[i-1][j] + grid[i+1][j]+ grid[i][j-1]+ grid[i][j+1])*0.25;
      }
    }
    double ** tmp = grid;
    grid = new;
    new = tmp;

  }
  double end_time = read_timer();
  printf("The execution time is %g sec\n", end_time - start_time);
  FILE *f = fopen("filedata.out", "w");
  for (i = 0; i < gridSize; i++) {
    for (j = 0; j < gridSize; j++) {
      fprintf(f,"%f ", grid[i][j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
  return 0;
}
