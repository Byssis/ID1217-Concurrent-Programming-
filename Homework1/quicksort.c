#include <stdio.h>
#include <stdlib.h>

void quicksort(int array[], int lo, int hi){
  if(lo >= hi) return;
  int p = partition(array, lo, hi);
  quicksort(array, lo, p-1);
  quicksort(array, p+1, hi);
}

int partition(int array[], int lo, int hi){
  int p = array[hi];
  int i = lo - 1;
  int j;
  for (j = lo; j < hi; j++){
    if( array [j] < p)
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

int main(){
  int array [] = {3,4,7,7,7,3,9,7,4,3,2,1,6,73,8};
  int size = (int)( sizeof(array) / sizeof(array[0]));
  //printf("%d\n", size);
  quicksort(array, 0, size-1);
  int i;
  for (i = 0; i < (int)( sizeof(array) / sizeof(array[0]))-1; i++) {
    printf("%d, ", array[i]);
  }
}
