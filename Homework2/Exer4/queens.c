#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXSIZE 16
#define MAXWORKERS 32

int isSolution(int *rows, int size){
  int i, j;

  for (i = 0; i < size; i++) {
    for (i = 0; i < size; i++) {
      // Same colum
      if(rows[i] == rows[j]) return 0;
        // Diagonal
      if(rows[i] - rows[j] == i - j || rows[i] - rows[j] == j - i) return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[]){
  int i, iteration, size, numWorkers, possibilities = 1, nummerOfSolutions = 0;
  double startTime, endTime;

  size = (argc > 1) ? atoi(argv[1]) : 8;
  size = (size <= MAXSIZE) ?  size : MAXSIZE;
  numWorkers = (argc > 2) ? atoi(argv[2]) : 4 ;
  numWorkers = (numWorkers <= MAXWORKERS) ? numWorkers : MAXWORKERS;

  omp_set_num_threads(numWorkers);

  for(i = 0; i < size; i++){
    possibilities *= size;
  }
  startTime = omp_get_wtime();
  for(iteration = 0; iteration < possibilities; iteration++){
    int j;
    int rows[size];
    int place = iteration;
    for(j = 0; j < size; j++){
      rows[j] = place % size;
      place /= size;
      //printf("%d\n", place);
    }

    if(isSolution(rows, size) == 1){
      nummerOfSolutions++;
      int row, col;
      for ( row = 0; row < size; row++) {
        for ( col = 0; col < size; col++){
          if(rows[row] == col)  printf("| Q ");
          else                  printf("|   ");
        }
        printf("|\n");
      }
      printf("\n");
    }
  }
  endTime = omp_get_wtime();

  printf("Num threads: %d. The execution time is %g sec. Num solutions: %d\n"
          , numWorkers, endTime - startTime, nummerOfSolutions);

}
