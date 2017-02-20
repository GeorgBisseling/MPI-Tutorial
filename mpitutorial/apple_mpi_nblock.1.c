/*
  DESC: rank 0 polls, other ranks send nonblocking - at least they try.
  This tries to be a little better than apple_mpi_poll.1.c by using
  MPI_Isend to send back the intermediate results to rank 0.
  Hint: Depending on your MPI implementation and device/interconnect
        that might not work as expected.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "invar.h"

int main (int argc, char **argv)
{
  int 		row, col, iter, keep_going, nprocs, myrank,
		numreqs = 0;
  int 		*result = 0;
  double 	re, im;
  MPI_Request 	*reqs = 0; /* recv for rank 0, send for everybody else */

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

  /* wastefully allocate memory for requests */
  reqs = (MPI_Request *) malloc (2 * YPIX * sizeof (MPI_Request));

  if (0 == myrank) {
      for (row = 0; row < YPIX; row++) {
	  MPI_Irecv(result + (row * XPIX), XPIX, MPI_INT,
		    MPI_ANY_SOURCE,
		    row,
		    MPI_COMM_WORLD, reqs + numreqs);
	  numreqs++;
      }
  }

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
	  MPI_Isend (result + row * XPIX, XPIX, MPI_INT,
		     0, row, MPI_COMM_WORLD, reqs + numreqs);
	  numreqs++;
	}

  /* complete all nonblocking communication: */
  MPI_Waitall (numreqs, reqs, MPI_STATUSES_IGNORE);

  MPI_Finalize ();

  if (0 == myrank) writeOut (argv[0], nprocs, result);
  return 0;
}
