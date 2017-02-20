# This file may use gnumake features.

# Set MPIHOME in your environment or here
# MPIHOME=/opt/intel/.....
MPIHOME=/usr

CC		= gcc
MPICC		= $(MPIHOME)/bin/mpicc
MPICCFLAGS 	= -O3 -Wall

# normal openmp compilation 
#OMPFLAGS	= -openmp 
# openmp compilation with preparation for using itcinstrument:
#OMPFLAGS	= -openmp -DUSEVT -I${VT_ROOT}/include -L${VT_ROOT}/slib -lVTnull
OMPFLAGS	= -fopenmp

# To get an instrumented binary do
# itcinstrument --input apple_omp.1 --output vtapple_omp.1 --profile on --insert libVT.so
# and ignore the warning regarding MPI libraries.

#LIBS		= -L${VT_ROOT}/slib -lm
LIBS		= -lm 
PROGS		= apple_serial \
		whoami \
		apple_pseudo_mpi.1 \
		apple_pseudo_mpi.2 apple_pseudo_mpi.2.ir \
		apple_pseudo_mpi.3 apple_pseudo_mpi.3.ir \
		apple_mpi.1 apple_mpi.1.ir \
		apple_mpi_poll.1 apple_mpi_poll.1.ir \
		apple_mpi_nblock.1 apple_mpi_nblock.1.ir \
		apple_mpi_nblock.2 apple_mpi_nblock.2.ir \
		apple_mpi_dynamic.1.ir \
		apple_omp.1 apple_omp.2

all:  progs

world: progs doc

progs: invar.o invar.ir.o $(PROGS)

# move all generated binaries to given dir (for use in build driver scripts)
move:
	mv invar.o invar.ir.o $(PROGS) $(MOVEDIR)

# print list of progs (for use in build driver scripts)
list:	
	@echo ${PROGS}


invar.o: invar.h invar.c
	$(MPICC) -c $(MPICCFLAGS) invar.c -o invar.o

# Just in case that -DIDLEROOT will at some point in time have an impact 
# on invar.c we compile a second version:
invar.ir.o: invar.h invar.c
	$(MPICC) -c $(MPICCFLAGS) -DIDLEROOT invar.c -o invar.ir.o

apple_omp.1: apple_omp.1.c invar.o
	$(MPICC) $(MPICCFLAGS) ${OMPFLAGS} $< invar.o -o $@ $(LIBS)	

apple_omp.2: apple_omp.2.c invar.o
	$(MPICC) $(MPICCFLAGS) ${OMPFLAGS} $< invar.o -o $@ $(LIBS)	


# I never can remeber those fancy rules of GNU make.
# Look into the documentation (Pattern Rules) to make
# some sense of it. They can be great: two days of head 
# scratching save you a whole day of typing!
%:  %.c invar.o invar.h
	$(MPICC) $(MPICCFLAGS) $< invar.o -o $@ $(LIBS)

%.ir: %.c invar.ir.o invar.h
	$(MPICC) $(MPICCFLAGS) -DIDLEROOT $< invar.ir.o -o $@ $(LIBS)

doc:
	cd sgml && make

.PHONY: clean veryclean

clean:
	rm -f $(PROGS) *.o

veryclean: clean
	rm -f core *~ *.gray
	cd sgml && make clean

#
