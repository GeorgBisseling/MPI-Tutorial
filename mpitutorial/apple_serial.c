/*
  DESC: apple_serial.c is a simple sequential program to start with.
*/

#include <stdio.h>
#include <stdlib.h>
#include "invar.h"

int main (int argc, char **argv)
{
  int		row, col, iter, keep_going;
  int 		*result = 0;
  double 	re, im;

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
      //fprintf (stdout, "%8d\r", row); fflush (stdout);
  }

  writeOut (argv[0], 1, result);
  return 0;
}
