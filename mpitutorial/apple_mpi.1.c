/*
  DESC: the first working MPI program.
  Despite the complexity the performance is not too rewarding.
  Reason is the (potentially) blocking MPI_Send that is used
  to report the rows back.
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
      if (0 == myrank)usage (argc, argv);
      MPI_Finalize ();
      exit (-1);
  }

  distributeRanges ();

  /* for simplicity we allow the full matrix in each rank: */
  result = (int *) malloc (XPIX * YPIX * sizeof (*result));

  /* set up the MPI_Irecvs needed. We use the tag of the
     MPI messages to let each result fall into place:
   */
  if (0 == myrank) {
      recv_reqs = (MPI_Request *) malloc (YPIX * sizeof (MPI_Request));
      for (row = 0; row < YPIX; row++)
	  MPI_Irecv(result + (row * XPIX), XPIX, MPI_INT,
		    MPI_ANY_SOURCE /* we do not care who does the job */ ,
		    row /* tag */ ,
		    MPI_COMM_WORLD, recv_reqs + row);
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
	  /* report row back to root: */
	  MPI_Send(result + row * XPIX, XPIX, MPI_INT,
		   0, row, MPI_COMM_WORLD);
      }

  /* complete all Irecvs: */
  if (0 == myrank) MPI_Waitall (YPIX, recv_reqs, MPI_STATUSES_IGNORE);

  MPI_Finalize ();

  if (0 == myrank) writeOut (argv[0], nprocs, result);
  return 0;
}
