#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <omp.h>

#define GRIDSIZE 10
#define NUMITERS 10
#define WORKERS 4
#define BOUNDRYCONSTANT 1.0

int main(int argc, char const *argv[]) {
  int gridSize, numIters, workers, i, j, iter, workload;

  gridSize = (argc > 1) ? atoi(argv[1]) : GRIDSIZE;
  numIters = (argc > 2) ? atoi(argv[2]) : NUMITERS;
  workers = (argc > 3) ? atoi(argv[3]) : WORKERS;
  workload = gridSize/workers;
  double ** grid = (double**)malloc(gridSize*sizeof(double));
  double ** new =  (double **)malloc(gridSize*sizeof(double));

  omp_set_num_threads(workers);

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


  double start_time =  omp_get_wtime();
#pragma omp parallel
{
  for(iter = 0; iter < numIters; iter++){
#pragma omp for private(j) schedule(static, workload)
    for (i = 1; i < gridSize - 1; i++) {
      for (j = 1; j < gridSize - 1; j++) {
        new[i][j] = (grid[i-1][j] + grid[i+1][j]+ grid[i][j-1]+ grid[i][j+1])*0.25;
      }
    } // implicit barrier

    double ** tmp = grid;
    grid = new;
    new = tmp;
  }
}
  double end_time =  omp_get_wtime();
  printf("The execution time is %g sec\n", end_time - start_time);
  FILE *f = fopen("filedata_openmp.out", "w");
  for (i = 0; i < gridSize; i++) {
    for (j = 0; j < gridSize; j++) {
      fprintf(f,"%f ", grid[i][j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
  return 0;
}
