all:
	gcc -o jacobi.out jacobi.c
	gcc -O -fopenmp -o jacobi_OPENMP.out jacobi_OPENMP.c

test1:
	./jacobi.out 100 100000
	./jacobi.out 200 100000

test2:
	./jacobi_OPENMP.out 100 100000 1
	./jacobi_OPENMP.out 100 100000 2
	./jacobi_OPENMP.out 100 100000 3
	./jacobi_OPENMP.out 100 100000 4
	./jacobi_OPENMP.out 200 100000 1
	./jacobi_OPENMP.out 200 100000 2
	./jacobi_OPENMP.out 200 100000 3
	./jacobi_OPENMP.out 200 100000 4
