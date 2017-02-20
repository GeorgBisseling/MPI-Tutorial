/*
  DESC: rank 0 polls for results, other ranks send blocking
  This tries to be a little better than apple_mpi.1.c by
  letting rank 0 poll for results during it's own
  calculation. This should prevent the other ranks from
  beeing blocked.
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
  MPI_Request 	*recv_reqs = 0;

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

  if (0 == myrank) {
      recv_reqs = (MPI_Request *) malloc (YPIX * sizeof (MPI_Request));
      for (row = 0; row < YPIX; row++)
	  MPI_Irecv(result + (row * XPIX), XPIX, MPI_INT,
		    MPI_ANY_SOURCE,
		    row,
		    MPI_COMM_WORLD, recv_reqs + row);
  }

#ifdef IDLEROOT
  if (myrank != 0)
#endif
    for (row = 0; row < YPIX; row++)
      if (MYROW(nprocs, myrank, row)) {
	  im = IM_START + row * (IM_STOP - IM_START) / (YPIX - 1.0);
	  for (col = 0; col < XPIX; col++) {
	      re = RE_START + col * (RE_STOP - RE_START) / (XPIX - 1.0);
	      iter = iterate (re, im, MAXITER);
	      result[row * XPIX + col] = iter;
	  }
	  MPI_Send(result + row * XPIX, XPIX, MPI_INT,
		   0, row, MPI_COMM_WORLD);
	  /* If root calculates it polls for results during calculation: */
#ifndef IDLEROOT
	  if (0 == myrank) {
	      int flag;
	      /* Does it make a difference? */
#if 0
	      int index;
	      MPI_Testany(YPIX, recv_reqs, &index, &flag, MPI_STATUSES_IGNORE);
#else
	      MPI_Testall(YPIX, recv_reqs,         &flag, MPI_STATUSES_IGNORE);
#endif
	    }
#endif
      }

  if (0 == myrank) MPI_Waitall (YPIX, recv_reqs, MPI_STATUSES_IGNORE);

  MPI_Finalize ();

  if (0 == myrank) writeOut (argv[0], nprocs, result);
  return 0;
}
