#include <stdio.h>
#include <stdlib.h>
#ifdef USEVT
#include <VT.h>
#endif
#include "invar.h"
int
main (int argc, char **argv)
{
    int		row, col, iter, keep_going;
    double 	re, im;
#ifdef COMPARE
    int 	*result_serial = 0;
#endif
    int 	*result_parallel = 0;

#ifdef USEVT
    VT_initialize(&argc, &argv);
#endif

    keep_going = initRanges (argc, argv);
    if (!keep_going) {
	usage (argc, argv);
	exit (-1);
    }

#ifdef COMPARE
    result_serial   = (int*) malloc(XPIX * YPIX * sizeof (*result_serial));
    for (row = 0; row < YPIX; row++) {
	im = IM_START + (row * (IM_STOP - IM_START)) / (YPIX - 1.0);
	for (col = 0; col < XPIX; col++) {
	    re = RE_START + (col * (RE_STOP - RE_START)) / (XPIX - 1.0);
	    iter = iterate(re, im, MAXITER);
	    result_serial[row * XPIX + col] = iter;
	}
    }
#endif

    result_parallel = (int*) malloc(XPIX * YPIX * sizeof (*result_parallel));
#   pragma omp parallel for \
               private(row, col, re, im, iter) \
               shared(result_parallel) \
	       schedule(dynamic, 1)
    for (row = 0; row < YPIX; row++) {
	im = IM_START + (row * (IM_STOP - IM_START)) / (YPIX - 1.0);
	for (col = 0; col < XPIX; col++) {
	    re = RE_START + (col * (RE_STOP - RE_START)) / (XPIX - 1.0);
	    iter = iterate(re, im, MAXITER);
	    result_parallel[row * XPIX + col] = iter;
	}
    }
#ifdef COMPARE
    for (row = 0; row < YPIX; row++) {
	for (col = 0; col < XPIX; col++) {
	    if (result_serial[row * XPIX + col] 
		!= result_parallel[row * XPIX + col] ) 
		return -1;
	}
    }
#endif

#ifdef USEVT
    VT_finalize();
#endif

    return 0;
}
