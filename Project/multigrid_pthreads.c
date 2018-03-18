#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define GRIDSIZE 10
#define NUMITERS 10
#define BOUNDRYCONSTANT 1.0
#define VCYCLE 4
#define ITERATIONSPERLEVEL 4
#define WORKERS 4

typedef struct args {
  double ** grid;
  double ** new;
  int gridSize;
  int start;
  int end;
  int id;
  int numIters;
} theard_args;


pthread_barrier_t barrier;
pthread_barrierattr_t attr;

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

void * Worker(void * args){
  int iter, i, j, id, v, start, end, step, numIters, gridSize, size;
  double ** grid,** new, **tmp;
  struct args * arg = (struct args *) args;
  id = arg->id;
  grid = arg->grid;
  new = arg->new;
  start = arg->start;
  end = arg->end;
  gridSize = arg->gridSize;
  numIters = arg->numIters;
  int s[VCYCLE];
  int e[VCYCLE];

  for (v = VCYCLE-1; v > 0 ; v--) {
    step = 1 << VCYCLE - 1 - v;
    if(v < VCYCLE-1){
      if(start % step > 0)
        start += (step - start % step);
      if(end % step > 0)
        end -= end % step;
    }
    if(start < step)
      start = step;
    s[VCYCLE - 1 - v] = start;
    e[VCYCLE - 1 - v] = end;
    printf("Id %d %d %d Step: %d\n", id, start, end, step);

    // Calulate jacobi
    for(iter = 0; iter < ITERATIONSPERLEVEL; iter++){
      for (i = start; i < end && i < (gridSize - step); i += step)
        for (j = step; j < gridSize - step; j += step )
          new[i][j] = (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.25;
      // Swap grid and new
      double ** tmp = grid; grid = new; new = tmp;
      // barrier*/
      pthread_barrier_wait(&barrier);
    }

    // Update next level coarse grid
    int next_step = step << 1;
    int next_start = start, next_end = end;

    if(next_start % next_step > 0)
      next_start += (next_step - next_start % next_step);
    if(next_end % next_step > 0)
      next_end -= next_end % next_step;

    if(next_start < next_step)
      next_start = next_step;

    printf("NEXT! Id %d %d %d Step: %d\n", id, next_start, next_end, next_step);
    for (i = next_start ; i < next_end && i < (gridSize - next_step); i += next_step)
      for (j = next_step; j < gridSize - next_step; j += next_step )
        new[i][j] = grid[i][j] + (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.125;

    // Swap grid and new
    double ** tmp = grid; grid = new; new = tmp;
    // barrier
    pthread_barrier_wait(&barrier);
  }

  step = 1 << VCYCLE - 1;
  if(v < VCYCLE-1){
    if(start % step > 0)
      start += (step - start % step);
    if(end % step > 0)
      end -= end % step;
  }
  if(start < step)
    start = step;
  //printf("%d Bottom\n", id);
  for(iter = 0; iter < numIters; iter++){
    for (i = start; i < end && i < (gridSize - step); i += step)
      for (j = step; j < gridSize - step; j += step )
        new[i][j] = (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.25;
    // Swap grid and new
    tmp = grid; grid = new; new = tmp;
    pthread_barrier_wait(&barrier);
    //printf("%d Leaving barrier\n", id);
  }

  //printf("%d Going up\n", id);
  for (v = 1; v < VCYCLE; v++) {
    int step = 1 << VCYCLE - 1 - v;
    start = s[VCYCLE-1-v];
    end = e[VCYCLE-1-v];
    if(start < 2*step)
      start = 2*step;

    printf("Id %d %d %d Step: %d\n", id, start, end, step);
    // Get fine grid from coarse grid
    // Fix colums
    for (i = start; i < end && i < gridSize; i += 2*step)
      for (j = 2*step; j < gridSize; j += 2*step)
        new[i-step][j] = (grid[i-2*step][j] + grid[i][j])*0.5;

    // ska man swapa här?!

    // Fix resut of fine garins
    for (i = 2*step; i < end && i < gridSize; i += 2*step)
      for (j = 2*step; j < gridSize; j += 2*step )
        new[i-step][j-step] = (grid[i-step][j-2*step] + grid[i-step][j])*0.5;
    // Swap grid and new
    tmp = grid; grid = new; new = tmp;
    pthread_barrier_wait(&barrier);
    start = s[VCYCLE-1-v];
    end = e[VCYCLE-1-v];
    if(start < 2*step)
      start = 2*step;

    // Calculate Jacobi iterations
    for(iter = 0; iter < ITERATIONSPERLEVEL; iter++){
      for (i = start; i < end && i < gridSize - step; i += step)
        for (j = step; j < gridSize - step; j += step )
          new[i][j] = (grid[i-step][j] + grid[i+step][j]+ grid[i][j-step]+ grid[i][j+step])*0.25;
      // Swap grid and new
      double ** tmp = grid; grid = new; new = tmp;
      // barrier
      pthread_barrier_wait(&barrier);
    }
  }

}

int main(int argc, char const *argv[]) {
  int  gridSize, numIters, i, j, iter, v, workers;

  double ** tmp;
  gridSize = (argc > 1) ? atoi(argv[1]) : GRIDSIZE;
  numIters = (argc > 2) ? atoi(argv[2]) : NUMITERS;
  workers = (argc > 3) ? atoi(argv[3]) : WORKERS;

  pthread_t workerid[workers];

  for (i = 1; i < VCYCLE; i++){
    printf("%d\n", gridSize)  ;
    gridSize = 2*gridSize + 1;
  }
  printf("%d\n", gridSize);
  pthread_barrier_init(&barrier, &attr, workers );

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
  int workload = gridSize/workers;
  int extra = gridSize - workload * workers;
  printf("%d %d\n", workload, extra);
  int prev = 0;
  for (i = 0; i < workers; i++) {
    struct args * arg = (struct args *)malloc(sizeof(struct args));
    arg->grid = grid;
    arg->new = new;
    arg->gridSize = gridSize;
    arg->start = prev;
    arg->id = i;
    arg->numIters = numIters;
    if(i == workers -1 )
      arg->end = gridSize;
    else{
      arg->end = arg->start + workload;
      if(extra >  0)
        arg->end++;
      prev = arg->end;
      extra--;
    }

    //printf("Id: %d, Start: %d, End: %d, Workload: %d, Extra: %d\n", i,arg->start, arg->end, arg->end - arg->start, extra);
    pthread_create(&workerid[i], NULL, Worker, (void *) arg);
  }

  for (i = 0; i < workers; i++) {
    pthread_join(workerid[i], NULL);
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
