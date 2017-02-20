/*
  DESC: calculating parallel and getting no results, continued.
  This is just a small improvement over mpiapple0.1.
  Only rank 0 will evaluate the command line arguments
  and we will use MPI_Bcast to pass the result of
  this evaluation to the other ranks. If the evaluation fails
  all ranks will terminate using MPI_Finalize in an orderly 
  fashion.
  We still do not gather the results in one place.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "invar.h"

int main (int argc, char **argv)
{
  int 		row, col, iter, keep_going, nprocs, myrank;
  int 		*result = 0;
  double 	re, im;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  if (nprocs < PROCLIMIT) {
      MPI_Finalize ();
      exit (-1);
  }

  if (0 == myrank) keep_going = initRanges (argc, argv);
  MPI_Bcast (&keep_going, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (!keep_going) {
      if (0 == myrank) usage (argc, argv);
      MPI_Finalize ();
      exit (-1);
  }

  distributeRanges ();

  result = (int *) malloc (XPIX * YPIX * sizeof (*result));

#ifdef IDLEROOT
  if (myrank != 0)
#endif
    for (row = 0; row < YPIX; row++)
      if (MYROW (nprocs, myrank, row)) {
	  im = IM_START + row * (IM_STOP - IM_START) / (YPIX - 1.0);
	  for (col = 0; col < XPIX; col++) {
	      re = RE_START + col * (RE_STOP - RE_START) / (XPIX - 1.0);
	      iter = iterate (re, im, MAXITER);
	      result[row * XPIX + col] = iter;
	  }
      }

  MPI_Finalize ();

  if (0 == myrank) writeOut (argv[0], nprocs, result);
  return 0;
}
