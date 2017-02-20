#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "invar.h"

#define TAG_WORK 0x0815
#define TAG_WORK_DONE 0x1704

/*
  DESC: just to show the mapping of ranks to <whatever_MPI_get_processor_name_returns>.
*/

int
main (int argc, char **argv)
{
  int rank;
  int nprocs, myrank;
  double time_after_mpi_init, time_before_mpi_finalize = 0;

  MPI_Init (&argc, &argv);
  time_after_mpi_init = MPI_Wtime ();
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myrank);

  time_before_mpi_finalize = MPI_Wtime ();
  for (rank = 0; rank < nprocs; rank++)
    {
      if (myrank == rank)
	{
	  char name[MPI_MAX_PROCESSOR_NAME];
	  int len = 0;
	  if (0 == myrank)
	    {
	      int flag;
	      void *value;

	      MPI_Attr_get (MPI_COMM_WORLD,
			    MPI_WTIME_IS_GLOBAL, &value, &flag);
	      fprintf (stdout, "MPI_Wtime is%sglobal\n",
		       flag ? " " : " not ");
	    }
	  MPI_Get_processor_name (name, &len);
	  fprintf (stdout, "rank %d, name %s, time %g\n",
		   myrank,
		   name, time_before_mpi_finalize - time_after_mpi_init);
	}
      fflush (stdout);
      MPI_Barrier (MPI_COMM_WORLD);
    }
  fflush (stdout);
  MPI_Barrier (MPI_COMM_WORLD);

  MPI_Finalize ();
  return 0;
}
