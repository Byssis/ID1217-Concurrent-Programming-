/*
  finding palindromic words in a dictionary using openmp

    usage with gcc (version 4.2 or higher required):
      gcc -O -fopenmp -o palindromic palindromic.c
      ./palindromic size numWorkers
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MAXWORKERS 24
#define WORDLENGTH 40
#define TASKLENGTH 10
#define MAXSIZE 30000

int sum = 0;
int size;
char dictionary[MAXSIZE][WORDLENGTH];
int mark[MAXSIZE];

/*
    reverse string
    char * word, string to reverse
*/
void reverse(char * word, char * r){
  int i, j;
  i = 0;
  j = strlen(word) - 1;
  while (j >= 0) {
    r[i++] = word[j--];
  }
  r[i]  = '\0';
}

/*
  binarySearch for searching in the dictionary
*/
int binarySearch(int l, int r, char * x){
  while (l <= r){
    int m = l + (r-l)/2;
    int result = (int)strcmp(x, dictionary[m]);
    if (result == 0)
      return m;
    if (result > 0)
      l = m + 1;
    else
      r = m - 1;
  }
  return -1;
}

void Workers(int size){
  int i;
  #pragma omp parallel for reduction (+:sum)
  for (i = 0; i < size; i++) {
    // 1. Get word
    char * word = dictionary[i];
    // 2. flip word
    char flip[WORDLENGTH];
    reverse(word, flip);
    // 3. search for fliped word in dictionary
    int result = binarySearch(0, size, flip);
    // 4. mark if in dictionary
    if(result != -1){
      sum++;
      mark[i] = 1;
      //mark[result] = 1;
    }
  }
}

int main(int argc, char *argv[]){
  double start_time, end_time;
  int k = 0, i, numWorkers;
  long l;

  if(argc < 3){
    printf("Error! Argument missing: file to examine\n");
    exit(0);
  }
  char const* const fileName = argv[1];
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  omp_set_num_threads(numWorkers);

  // Read dictionary
  FILE* file = fopen(fileName, "r");
  while(fscanf(file,"%s",dictionary[k]) == 1){
    for(i= 0; dictionary[k][i]; i++){
        dictionary[k][i] = tolower(dictionary[k][i]);
    }
    mark[k] = 0;
    k++;
  }
  fclose(file);
  size = k - 1;

  start_time = omp_get_wtime();                  // Start time for benchmark
  Workers(size);
  end_time = omp_get_wtime();                    // End time for benchmark

  FILE* resultfile = fopen("Result.txt", "w");
  for(i = 0; i < size; i++){
    if(mark[i]){
      fprintf(resultfile, "%s\n", dictionary[i]);
    }
  }
  printf("Result stored in Result.txt\n");

  // Print execution time
  printf("Num threads: %d. The execution time is %g sec. Num words: %d\n"
          , numWorkers, end_time - start_time, sum);

}
