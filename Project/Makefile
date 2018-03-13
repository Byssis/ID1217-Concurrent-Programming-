all:
	gcc -o jacobi.out jacobi.c
	gcc -O -fopenmp -o jacobi_OPENMP.out jacobi_OPENMP.c

test1:
	./jacobi.out 100 100
	./jacobi.out 200 100

test2:
	./jacobi_OPENMP.out 100 100 1
	./jacobi_OPENMP.out 100 100 2
	./jacobi_OPENMP.out 100 100 3
	./jacobi_OPENMP.out 100 100 4
	./jacobi_OPENMP.out 200 100 1
	./jacobi_OPENMP.out 200 100 2
	./jacobi_OPENMP.out 200 100 3
	./jacobi_OPENMP.out 200 100 4
