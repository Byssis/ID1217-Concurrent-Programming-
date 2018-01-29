#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ERROR 0.1

double fun(double x){
  return sqrt( 1.0 - x * x );
}

double quad(double l, double r, double fl, double fr, double area){
  double m, fm, larea, rarea;
  m = ( l + r ) / 2;
  fm = fun(m);
  larea = ( fl + fm ) * ( m - l ) / 2;
  rarea = ( fl + fr ) * ( r - m ) / 2;
  if(abs((larea + rarea) - area) > ERROR){
    larea = quad(l, m, fl, fm, larea);
    rarea = quad(m, r, fm, fr, rarea);
  }
  return (larea + rarea);
}

int main() {
  double a, b, area, fa, fb,result;
  a = 0;
  b = 1;
  fa = fun(a);
  fb = fun(b);
  area = ( fa + fb ) * ( fb - fa ) / 2;
  result = quad(a, b, fa, fb, area);
  printf("%d\n", result);
  return 0;
}