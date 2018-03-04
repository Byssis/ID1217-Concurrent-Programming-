/*
  Find common values in T different array. Each array is represented
  by a process. Solution implements a circular pipeline. Process 0 (root process) start by sending one value at time
  to process 1. Process 1 checks if received value is also in process 1 array.
  If true it will send the received to the next process.

  When process 0 is done sending all values it signals that all values are
  sent to process 1 and process 1 will signal process 2 and so on.

  If process 0 receives any value, it means it is a common value for all
  arrays. It will then send it to the next process.

  Process 0 will send to 1 and receive from T-1
  Process T-1 will send to 0 and receive from T-2
  Process I will send to I+1 and receive from I-1


  Compile:
    mpicc thewelfarecrook.c -o thewelfarecrook

  Run:
    mpirun -np 3 ./thewelfarecrook
*/
#include	"mpi.h"
#include <stdio.h>

#define LENGTH 100
#define NUMBER_OF_PROCESSES 3

int main(int argc, char* argv[]){
  int rank, size, send, receive, number, length, i, j, num_of_proc;

  num_of_proc = (argc > 1) ? atoi(argv[2]) : NUMBER_OF_PROCESSES;
  length = (argc > 2) ? atoi(argv[3]) : LENGTH;

  int values [num_of_proc][length];

  for(i = 0; i < num_of_proc; i++){
    for (j = 0; j < length; j++) {
      values[i][j] = rand() % (length*5);
    }
  }

  MPI_Init (&argc, &argv);                      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

  send = (rank + 1) % size;
  receive = (rank == 0 ) ? size - 1 : (rank - 1) % size;

  printf( "ID %d:\tSend to %d, Receive from %d\n", rank, send, receive);
  if(rank == 0){
    for(i = 0; i < length; i++)
      MPI_Send( &values[rank][i],	1, MPI_INT, send,	0, MPI_COMM_WORLD);
    number = -1;
    MPI_Send( &number, 1, MPI_INT, send, 0, MPI_COMM_WORLD);
  }else{
    while(1){
      MPI_Recv( &number, 1, MPI_INT, receive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      //printf( "ID %d:\tReceived %d from %d\n", rank, number, receive);
      if(number == -1) {
        MPI_Send( &number, 1, MPI_INT, send, 0, MPI_COMM_WORLD);
        break;
      }
      for(i = 0; i < length; i++){
        if(values[rank][i] == number){
          MPI_Send( &number,	1, MPI_INT, send,	0, MPI_COMM_WORLD);
          printf( "ID %d:\tBoth %d and %d have %d\n", rank, rank, receive, number);
          break;
        }
      }
    }
  }
  while(1){
    MPI_Recv( &number, 1, MPI_INT, receive, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(number == -1){
      MPI_Send( &number,	1,	MPI_INT	,	send,	0,	MPI_COMM_WORLD);
      break;
    }
    else{
      MPI_Send( &number,	1,	MPI_INT	,	send,	0,	MPI_COMM_WORLD);
      printf( "ID %d:\tCommon numbers: %d\n", rank, number);
    }
  }
  MPI_Finalize();
}
