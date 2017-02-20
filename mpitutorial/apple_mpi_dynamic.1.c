/*
  DESC: dynamic load balancing
  This demonstrates how to adapt to varying calculation times due
  to imbalance in work packets (rows) or due to varying or
  asymetric computing power in the processors.
  This uses a master-slaves scheme.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "invar.h"

#define TAG_WORK	0x0815 /* flags an order to a slave */
#define TAG_WORK_DONE	0x1704 /* flags a result from a slave */

int main (int argc, char **argv)
{
  int 		col, iter, keep_going = 1, nprocs, myrank, rank;
  int 		*result = 0;
  int 		*lcount = 0; /* lcount[p] == #rows processor p did */

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
      if (0 == myrank)usage (argc, argv);
      MPI_Finalize ();
      exit (-1);
  }

  distributeRanges ();

  if (0 == myrank) {
      /* master */
      int i;
      int numbusy = 0, nextline = YPIX - 1;
      int *busy = 0; /* busy[p] != 0 means processor p is busy */
      MPI_Request *rreq = 0;
      int *completed_req_indices = 0; /* used in MPI_Waitsome */
      
      lcount = (int *) malloc (nprocs * sizeof (*lcount));
      memset (lcount, 0, nprocs * sizeof (*lcount));

      result = (int *) malloc (XPIX * YPIX * sizeof (*result));

      /* allocate mem in block local vars */
      busy = (int *) malloc (nprocs * sizeof (*busy));
      memset (busy, 0, nprocs * sizeof (*busy));

      rreq = (MPI_Request *) malloc (nprocs * sizeof (*rreq));
      /* you can not do that standard compliant with memset() */
      for (i = 0; i < nprocs; i++) rreq[i] = MPI_REQUEST_NULL;

      completed_req_indices = (int*)calloc(nprocs, sizeof(int));
      
      /* loop while there is work to do or some ranks are still working */
      while (nextline >= 0 || numbusy > 0) {
	  /* send out work */
	  while (nextline >= 0 && numbusy < nprocs - 1) {
	      for (rank = nprocs-1; rank > 0; rank--)
		  if (0 == busy[rank]) {
		      busy[rank] = 1;
		      /* slave listens, so blocking send is ok */
		      MPI_Send(&nextline, 1, MPI_INT, rank, TAG_WORK,
				MPI_COMM_WORLD);
		      MPI_Irecv(result + nextline * XPIX, XPIX, MPI_INT,
				rank, TAG_WORK_DONE, MPI_COMM_WORLD,
				rreq + rank);
		      nextline--;
		      numbusy++;
		      break;
		  }
	  }
	  /* try to receive something */
	  if (numbusy > 0) {
	      int who;
	      int outcount=0;
	      MPI_Waitsome(nprocs, rreq, &outcount, completed_req_indices,
			   MPI_STATUSES_IGNORE);
	      if (MPI_UNDEFINED != outcount)
	      for(int i=0; i<outcount; i++) {
		who = completed_req_indices[i];
		busy[who] = 0;
		lcount[who]++;
		numbusy--;
	      }
	  }
      }

      /* send all slaves the signal to stop: */
      for (rank = 1; rank < nprocs; rank++)
	  MPI_Send (&nextline, 1, MPI_INT, rank, TAG_WORK, MPI_COMM_WORLD);
      
  } else {
      /* slave(s) */
      int row;

      result = (int *) malloc (XPIX * sizeof (*result));

      while (keep_going) {
	  MPI_Recv (&row, 1, MPI_INT, 0, TAG_WORK, MPI_COMM_WORLD,
		    MPI_STATUS_IGNORE);
	  if (-1 == row) {
	      keep_going = 0;
	  } else {
	      im = IM_START + (row * (IM_STOP - IM_START)) / (YPIX - 1.0);
	      for (col = 0; col < XPIX; col++) {
		  re = RE_START + (col * (RE_STOP - RE_START)) / (XPIX - 1.0);
		  iter = iterate (re, im, MAXITER);
		  result[col] = iter;
	      }
	      MPI_Send(result, XPIX, MPI_INT, 
		       0, TAG_WORK_DONE, MPI_COMM_WORLD);
	  }
      }
  }

  char **names = getProcessorNames(MPI_COMM_WORLD);
  if (0 == myrank) {
    int r;
    for(r=0; r<nprocs; r++)
      fprintf(stdout, "%4d on %-10s computed %5d lines (%6.2f%%)\n", r, names[r], lcount[r], lcount[r] * 100.0 / YPIX);
  }

  fflush(stdout);

  MPI_Finalize ();

  if (0 == myrank) writeOut (argv[0], nprocs, result);
  return 0;
}
