#include <stdlib.h>
#include <stdio.h>
int ifsorted(int * array, int size){
    int prev = *(array++);
    int i = 0;
    for (i = 1; i < size; i++) {
      if(*array < prev) return 0;
      array++;
    }
    return 1;
}
