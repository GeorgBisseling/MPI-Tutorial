#! /bin/sh


MPIRUN="time /usr/local/mpich-1.2.6-IA32-ch_shmem/bin/mpirun"

for nprocs in 1; do
	for prog in mpiapple0.0 ; do
	    VT_PROGNAME="$prog.np$nprocs"
	    export VT_PROGNAME
	    echo $VT_PROGNAME
	    $MPIRUN -np $nprocs $prog
	done
done

for nprocs in 1 2 4 8; do
	for prog in mpiapple1.? ; do
	    VT_PROGNAME="$prog.np$nprocs"
	    export VT_PROGNAME
	    echo $VT_PROGNAME
	    $MPIRUN -np $nprocs $prog
	done
done

for nprocs in 2 3 5 9; do
	for prog in mpiapple1.?.ir ; do
	    VT_PROGNAME="$prog.np$nprocs"
	    export VT_PROGNAME
	    echo $VT_PROGNAME
	    $MPIRUN -np $nprocs $prog
	done
done
