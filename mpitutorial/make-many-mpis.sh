#! /bin/sh


ALLMPIS="/usr/local/lam-7.1.1-intel9-shared/ \
/usr/local/mpich-1.2.6-IA32-ch_p4/ \
/usr/local/mpich-1.2.6-IA32-ch_shmem/ \
/usr/local/mpich2-1.0.4p1-intel9/"


for i in $ALLMPIS; do
    MPIHOME=$i
    echo "************************************************************"
    echo "*** MAKE"
    echo "*** MPIHOME=$MPIHOME"
    echo "************************************************************"
    export MPIHOME
    make clean
    make progs 2>&1 | tee make.log

    dirname=`echo $MPIHOME | sed -e 's/\//_/g' `
    echo "************************************************************"
    echo "*** MOVE"
    echo "*** dirname=$dirname"
    echo "************************************************************"
    MOVEDIR=$dirname
    export MOVEDIR
    rm -fr $MOVEDIR
    mkdir $MOVEDIR
    make move
    mv make.log $MOVEDIR
    echo "MPIHOME=$MPIHOME; export MPIHOME" >$MOVEDIR/sourceme.sh
    echo "setenv MPIHOME $MPIHOME" >$MOVEDIR/sourceme.csh
    chmod +x $MOVEDIR/sourceme*

done
