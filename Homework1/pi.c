#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "bench.h"

pthread_mutex_t lock;

double error;
int num_workers;

double fun(double x){
  double result;
  result = 1.0 - x*x;
  result = sqrt( result );
  return result;
}

double dabs(double x){
  if(x > 0.0)   return x;
  else          return x * -1.0;
}

int i = 0;

double quad(double l, double r, double fl, double fr, double area){
  double m, fm, larea, rarea, a;
  m = ( l + r ) / 2.0;
  fm = fun(m);
  larea = ( fl + fm ) * ( m - l ) / 2.0;
  rarea = ( fl + fr ) * ( r - m ) / 2.0;
  a = dabs((larea + rarea) - area);
  if(dabs((larea + rarea) - area) > error){
    larea = quad(l, m, fl, fm, larea);
    rarea = quad(m, r, fm, fr, rarea);
  }
  return (larea + rarea);
}

int main(int argc, char *argv[]) {
  double a, b, area, fa, fb,result, start_time, end_time;
  error = 0.00000000001;//(double)atoi(argv[1]);
  start_time = read_timer();
  a = 0.0;
  b = 1.0;
  fa = fun(a);
  fb = fun(b);
  area = ( fa + fb ) * ( fb - fa ) / 2.0;
  result = 4 * quad(a, b, fa, fb, area);
  end_time = read_timer();
  printf("%f\n", result);
  printf("The execution time is %g sec\n", end_time - start_time);
  return 0;
}
