/*
  DESC: using MPI only for its overhead.
  Just demonstrate the overhead in initialization and
  finalization that MPI introduces.
  This program makes no intelligent use of more than
  one processor: if you run it on n>=1 CPUs then it
  will do the whole work on each CPU.
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

  keep_going = initRanges (argc, argv);
  if (!keep_going) {
      usage (argc, argv);
      exit (-1);
  }

  result = (int *) malloc (XPIX * YPIX * sizeof (*result));

  for (row = 0; row < YPIX; row++) {
      im = IM_START + row * (IM_STOP - IM_START) / (YPIX - 1.0);
      for (col = 0; col < XPIX; col++) {
	  re = RE_START + col * (RE_STOP - RE_START) / (XPIX - 1.0);
	  iter = iterate (re, im, MAXITER);
	  result[row * XPIX + col] = iter;
      }
      fprintf (stdout, "%8d\r", row);
      fflush (stdout);
  }
  fprintf (stdout, "\n");

  MPI_Finalize ();

  writeOut (argv[0], nprocs, result);
  return 0;
}
