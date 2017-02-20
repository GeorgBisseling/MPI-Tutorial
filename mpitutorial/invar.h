#ifndef _INVAR_H_
#define _INVAR_H_

#include <mpi.h>

/* Please see invar.c for a comment. */

void usage (int argc, char **argv);
int initRanges (int argc, char **argv);
void distributeRanges (void);
int iterate (double re, double im, int maxiter);
void writeOut (char *progname, int nprocs, int *result);
char** getProcessorNames(MPI_Comm comm);

#ifndef INVAR_EXTERN
#define INVAR_EXTERN extern
#endif

INVAR_EXTERN double RE_START;
INVAR_EXTERN double RE_STOP;
INVAR_EXTERN double IM_START;
INVAR_EXTERN double IM_STOP;

INVAR_EXTERN int XPIX;
INVAR_EXTERN int YPIX;
INVAR_EXTERN int MAXITER;

#undef INVAR_EXTERN

/* 
   These are needed in mpiapple0.1 and above to distribute
   the rows to ranks. Notice that we can decide if rank 0
   should participate in the calculation or not. This is
   used to implement a master/slave scheme.
*/
#ifdef IDLEROOT
#define MYROW(_nprocs, _myrank, _row) (_myrank - 1 == _row % (_nprocs - 1))
#define PROCLIMIT 2
#else
#define MYROW(_nprocs, _myrank, _row) (_myrank == _row % _nprocs)
#define PROCLIMIT 1
#endif


#endif
