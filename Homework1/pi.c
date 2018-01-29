#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define ERROR 100000

double fun(double x){
  double result;
  result = 1.0 - x*x;
  result = sqrt( result );
  return result;
}

int i = 0;

double quad(double l, double r, double fl, double fr, double area){
  double m, fm, larea, rarea;
  m = ( l + r ) / 2;
  fm = fun(m);
  larea = ( fl + fm ) * ( m - l ) / 2;
  rarea = ( fl + fr ) * ( r - m ) / 2;
  if(i++ < ERROR){
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
  result = 4 * quad(a, b, fa, fb, area);
  printf("%f\n", result);
  return 0;
}
