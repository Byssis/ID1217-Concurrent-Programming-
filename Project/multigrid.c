#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define GRIDSIZE 10
#define NUMITERS 10
#define BOUNDRYCONSTANT 1.0
#define VCYCLE 4
#define ITERATIONSPERLEVEL 4

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

void printgrid(double ** grid, int size, int step){
  int i, j;
  for (i = 0; i < size; i+=step)
    for (j = 0; j < size; j+=step)
      printf("%f ", grid[i][j]);
    printf("\n");
}

// Jacobi iterations
void jacobi(double ** grid, double ** new, int numIters, int size, int step){
  int iter, i, j;
  for(iter = 0; iter < numIters; iter++){
    for (i = step; i < size - step; i += step)
      for (j = step; j < size - step; j += step )
        new[i][j] = (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.25;
    // Swap grid and new
    double ** tmp = grid; grid = new; new = tmp;
  }
}

int main(int argc, char const *argv[]) {
  int  gridSize, numIters, i, j, iter, v;

  double ** tmp;
  gridSize = (argc > 1) ? atoi(argv[1]) : GRIDSIZE;
  numIters = (argc > 2) ? atoi(argv[2]) : NUMITERS;

  for (i = 1; i < VCYCLE; i++)
    gridSize = 2*gridSize + 1;

  // Init grid
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

  // Start time
  double start_time = read_timer();
  // Go down
  for (v = VCYCLE-1; v > 0 ; v--) {
    int step = 1 << VCYCLE-1 - v;
    // Calulate jacobi
    jacobi(grid, new, ITERATIONSPERLEVEL, gridSize, step);

    // Update next level coarse grid
    int next_step = step << 1;
    for (i = next_step; i < gridSize - next_step; i += next_step)
      for (j = next_step; j < gridSize - next_step; j += next_step )
        new[i][j] = grid[i][j] + (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.125;

    // Swap grid and new
    tmp = grid; grid = new; new = tmp;
  }

  int step = 1 << VCYCLE - 1;
  // Course grid (Bottom level of V)
  jacobi(grid, new, numIters, gridSize, step);

  // Go back up
  for (v = 1; v < VCYCLE; v++) {
    int step = 1 << VCYCLE - 1 - v;

    // Get fine grid from coarse grid
    // Fix colums
    for (i = 2*step; i < gridSize; i += 2*step)
      for (j = 2*step; j < gridSize; j += 2*step)
        new[i-step][j] = (grid[i-2*step][j] + grid[i][j])*0.5;

    // ska man swapa här?!

    // Fix resut of fine garins
    for (i = 2*step; i < gridSize; i += 2*step)
      for (j = 2*step; j < gridSize; j += 2*step )
        new[i-step][j-step] = (grid[i-step][j-2*step] + grid[i-step][j])*0.5;
    // Swap grid and new
    tmp = grid; grid = new; new = tmp;

    // Calculate Jacobi iterations
    jacobi(grid, new, ITERATIONSPERLEVEL, gridSize, step);
  }

  double end_time = read_timer();
  double max = 0.0;
  for (i = 1; i < gridSize - 1; i++) {
    for (j = 1; j < gridSize - 1; j++) {
      double diff = (grid[i][j] - new[i][j]);
      if(diff < 0)
        diff = -1*diff;
      if(diff > max)
        max = diff;
    }
  }
  printf("The execution time is %g sec, max diff %f\n", end_time - start_time, max);
  FILE *f = fopen("filedata_multigrid.out", "w");
  for (i = 0; i < gridSize; i++) {
    for (j = 0; j < gridSize; j++)
      fprintf(f,"%f ", grid[i][j]);
    fprintf(f, "\n");
    free(grid[i]);
    free(new[i]);
  }
  free(grid);
  free(new);
  fclose(f);
  return 0;
}
