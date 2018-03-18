all:
	gcc -o jacobi.out jacobi.c
	gcc -O -fopenmp -o jacobi_OPENMP.out jacobi_OPENMP.c
	gcc -o multigrid.out multigrid.c
	gcc -O -fopenmp -o multigrid_OPENMP.out multigrid_OPENMP.c
	gcc -o multigrid_pthreads.out multigrid_pthreads.c -lpthread

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
