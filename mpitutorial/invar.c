/* define global vars: */
#define INVAR_EXTERN		/* */

#include "invar.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

/* Tell what command line arguiments we understand. */
void
usage (int argc, char **argv)
{
    char *name = "<progname>";
    if (argc>0) name = argv[0];
    fprintf (stderr,
	     "Usage is either:\n"
	     "%s XPIX RE_START RE_STOP YPIX IM_START IM_STOP MAXITER\n"
	     "to set everything.\n\n", name);
  fprintf (stderr, "Or:\n" "%s <n>\n" "to set only MAXITER=<n>\n\n", name);
  fprintf (stderr,
	   "Or simply:\n"
	   "%s\n" "to leave everything at its default.\n", name);
}

/* Init the rectangle in the complex plane, the resolution and
   the maximum number of iterations per pixel.
*/
int
initRanges (int argc, char **argv)
{
  int keep_going = 1;		// ==1 mwans parsing ok

  RE_START = -2.0;
  IM_START = 0.0;
  RE_STOP = IM_STOP = +2.0;
  XPIX = 2048;
  YPIX = 1024;
  MAXITER = 4096;

  if (8 == argc) {
      if (1 != sscanf (argv[1], "%d", &XPIX))
	keep_going = 0;
      if (1 != sscanf (argv[2], "%lg", &RE_START))
	keep_going = 0;
      if (1 != sscanf (argv[3], "%lg", &RE_STOP))
	keep_going = 0;
      if (1 != sscanf (argv[4], "%d", &YPIX))
	keep_going = 0;
      if (1 != sscanf (argv[5], "%lg", &IM_START))
	keep_going = 0;
      if (1 != sscanf (argv[6], "%lg", &IM_STOP))
	keep_going = 0;
      if (1 != sscanf (argv[7], "%d", &MAXITER))
	keep_going = 0;
  } else if (2 == argc) {
      if (1 != sscanf (argv[1], "%d", &MAXITER))
	keep_going = 0;
  } else if (1 == argc) {
      ;
  } else {
      keep_going = 0;
  }

  if (keep_going) {
      fprintf(stdout, "RE_START = %40.30g\n", RE_START);
      fprintf(stdout, "RE_STOP  = %40.30g\n", RE_STOP);
      fprintf(stdout, "IM_START = %40.30g\n", IM_START);
      fprintf(stdout, "IM_STOP  = %40.30g\n", IM_STOP);
      fprintf(stdout, "XPIX     = %40d\n",    XPIX);
      fprintf(stdout, "YPIX     = %40d\n",    YPIX);
      fprintf(stdout, "MAXITER  = %40d\n",    MAXITER);
      fflush (stdout);
    }

  return keep_going;
}

void
distributeRanges (void)
{
  MPI_Bcast(&XPIX,     1, MPI_INT,    0, MPI_COMM_WORLD);
  MPI_Bcast(&RE_START, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&RE_STOP,  1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&YPIX,     1, MPI_INT,    0, MPI_COMM_WORLD);
  MPI_Bcast(&IM_START, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&IM_STOP,  1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&MAXITER,  1, MPI_INT,    0, MPI_COMM_WORLD);
}

int
iterate (double re, double im, int maxiter)
{
  double x = 0, y = 0, x2 = 0, y2 = 0;
  int k = 0;

  while ((k < maxiter) && (x2 + y2 <= ((double) 4.0)))
    {
      y = ((double) 2.0) * x * y + im;
      x = x2 - y2 + re;
      x2 = x * x;
      y2 = y * y;
      k++;
    }

  return k;
}

/* write it out as a 16 bit grayscale image suitable
   to be displayed with ImageMagick like this:
   display -depth 16 -size XPIYxYPIX <name>
   Use Enhance->Equalize to see something.
   This is not to really look terribly nice 
   but only to have a way to control that the
   calculated results look reasonable.
*/
void
writeOut (char *progname, int nprocs, int *result)
{
  char fname[256] = { 0 };
  FILE *fp;
  int row, col;
  unsigned int truncated;
  unsigned char clow, chigh;

  if (0 == getenv ("WRITEOUT"))
    return;

  sprintf(fname, 
	  "%s_%dp_size%dx%d.gray", 
	  basename(progname), nprocs, XPIX, YPIX);
  fprintf (stdout, "Writing result to file \"%s\".\n", fname);

  fp = fopen (fname, "w");
  for (row = YPIX - 1; row >= 0; row--)
      for (col = 0; col < XPIX; col++) {
	  /* The "% MAXITER" makes the inside of the apple black
	     as I've gotten used to that. At least for non eatable
	     apples. */
	  truncated = ((result[row * XPIX + col] % MAXITER) % 0x10000);
	  clow  = (unsigned char) (truncated & 0xff);
	  chigh = (unsigned char) ((truncated >> 8) & 0xff);
	  fwrite(&chigh, 1, 1, fp);
	  fwrite(&clow,  1, 1, fp);
      }
  fclose (fp);
}

char** getProcessorNames(MPI_Comm comm)
{
  int rank, myrank, nprocs;
  MPI_Comm_size (comm, &nprocs);
  MPI_Comm_rank (comm, &myrank);

  char name[MPI_MAX_PROCESSOR_NAME];
  int len = 0;
  MPI_Get_processor_name (name, &len);

  if (len>=MPI_MAX_PROCESSOR_NAME) {
    printf("ooops");
    abort();
  }

  name[len] = 0;

  fflush(stdout);
  
  if (myrank == 0) {
    char *recv_buff = (char *)calloc(nprocs, MPI_MAX_PROCESSOR_NAME);
    MPI_Gather(name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	       recv_buff, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	       0, comm);
    char** names = (char**) calloc(nprocs, sizeof(char*));
    for(rank=0; rank<nprocs; rank++) {
      names[rank] = (char*)malloc(MPI_MAX_PROCESSOR_NAME);
      strncpy(names[rank], recv_buff + rank*MPI_MAX_PROCESSOR_NAME, MPI_MAX_PROCESSOR_NAME);
    }
    free(recv_buff);
    return names;
  } else {
    MPI_Gather(name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR,
	       NULL, 0, MPI_CHAR,
	       0, comm);
    return NULL;
  }
}
